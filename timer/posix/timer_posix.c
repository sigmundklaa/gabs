
#include <stdatomic.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <stdatomic.h>

#include <posix_init.h>

#include <gabs/core/util.h>
#include <gabs/timer.h>

#include "atomic_mask.h"

#define BIT_USED   (1 << 0) /* Set by client, unset by worker */
#define BIT_ZOMBIE (1 << 1) /* Set by client, unset by worker */
#define BIT_ACTIVE (1 << 2) /* Set/unset by worker, unset by client */
#define BIT_SCHED  (1 << 3) /* Set by client, unset by worker */

#ifndef GABS_CONFIG_TIMER_POSIX_MAX
#define GABS_CONFIG_TIMER_POSIX_MAX (10)
#endif

#ifndef GABS_CONFIG_TIMER_INIT_PRIORITY
#define GABS_CONFIG_TIMER_INIT_PRIORITY (30)
#endif

struct timer_posix {
        gabs_timer_cb cb;
        void *user_arg;

        int fd;
        atomic_uint state;
};

static struct timer_posix timers[GABS_CONFIG_TIMER_POSIX_MAX];
static int event_fd;
static pthread_t thread_h;
static atomic_uint work_state;

enum {
        WORK_STATE_NORMAL,
        WORK_STATE_EXIT,
};

static void trigger_reset(void)
{
        uint64_t count;
        int size;

        count = 1;

        size = write(event_fd, &count, sizeof(count));
        assert(size == sizeof(count));
}

static void to_itimerspec(struct itimerspec *spec, uint32_t time_us)
{
        spec->it_interval.tv_sec = 0;
        spec->it_interval.tv_nsec = 0;

        spec->it_value.tv_sec = time_us / (uint32_t)1e6;
        spec->it_value.tv_nsec = (time_us % (uint32_t)1e6) * 1000;

        if (spec->it_value.tv_sec == 0 && spec->it_value.tv_nsec == 0) {
                spec->it_value.tv_nsec = 1;
        }
}

static gabs_timer timer_to_id(struct timer_posix *t)
{
        if (t == NULL || t > &timers[gabs_array_size(timers) - 1]) {
                return -1;
        }

        return t - timers;
}

static struct timer_posix *timer_from_id(gabs_timer id)
{
        if (id >= gabs_array_size(timers) || id < 0) {
                return NULL;
        }

        return &timers[id];
}

static int timer_restart(struct timer_posix *t, uint32_t delay_us)
{
        int status;
        struct itimerspec spec;

        /* Make sure timer does not fire and/or start before we schedule
         * it again. */
        mask_clear(&t->state, BIT_ACTIVE | BIT_SCHED);

        to_itimerspec(&spec, delay_us);

        status = timerfd_settime(t->fd, 0, &spec, NULL);

        mask_set(&t->state, BIT_SCHED);
        trigger_reset();

        if (status < 0) {
                return -errno;
        }

        return status;
}

static bool timer_valid(struct timer_posix *t)
{
        return mask_get(&t->state, BIT_USED | BIT_ZOMBIE) == BIT_USED;
}

static void timer_cleanup(struct timer_posix *t)
{
        close(t->fd);

        /* Reset state to default */
        atomic_store(&t->state, 0);
}

static struct timer_posix *timer_from_fd(int fd)
{
        struct timer_posix *t;
        size_t i;

        for (i = 0; i < gabs_array_size(timers); i++) {
                t = &timers[i];

                if (!timer_valid(t)) {
                        continue;
                }

                if (t->fd == fd) {
                        return t;
                }
        }

        return NULL;
}

static struct timer_posix *timer_alloc(void)
{
        struct timer_posix *cur;
        size_t i;

        for (i = 0; i < gabs_array_size(timers); i++) {
                cur = &timers[i];

                if (mask_set_strict(&cur->state, BIT_USED)) {
                        return cur;
                }
        }

        return NULL;
}

static struct timer_posix *timer_add(gabs_timer_cb cb, void *user_data)
{
        struct timer_posix *t;

        t = timer_alloc();
        if (t == NULL) {
                return NULL;
        }

        t->cb = cb;
        t->user_arg = user_data;
        t->state = 0;
        t->fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

        if (t->fd < 0) {
                return NULL;
        }

        mask_set(&t->state, BIT_USED);

        return t;
}

static void timer_alarm(struct timer_posix *t)
{
        if (t->cb == NULL) {
                return;
        }

        t->cb(timer_to_id(t), t->user_arg);
}

static void pfd_push(struct pollfd **arrptr, int fd)
{
        struct pollfd *pfd;

        pfd = *arrptr;
        pfd->fd = fd;
        pfd->events = POLLIN;
        pfd->revents = 0;

        *arrptr += 1;
}

/** @brief Prepare pollfds for those timers that require it */
static void prepare_pfds(struct pollfd **pfdptr)
{
        size_t i;
        struct timer_posix *cur;

        for (i = 0; i < gabs_array_size(timers); i++) {
                cur = &timers[i];
                if (!mask_test(&cur->state, BIT_USED)) {
                        continue;
                }

                if (mask_test(&cur->state, BIT_ZOMBIE)) {
                        timer_cleanup(cur);
                        continue;
                }

                /* Set active if scheduled bit is set */
                if (mask_test(&cur->state, BIT_ACTIVE) ||
                    mask_cas(&cur->state, BIT_SCHED, BIT_ACTIVE)) {
                        pfd_push(pfdptr, cur->fd);
                        mask_clear(&cur->state, BIT_SCHED);
                }
        }
}

static void service_pfds(struct pollfd *pfd, size_t count)
{
        struct timer_posix *cur;
        size_t i;
        uint64_t dummy;

        for (i = 0; i < count; i++, pfd++) {
                if (!(pfd->revents & POLLIN)) {
                        continue;
                }

                (void)read(pfd->fd, &dummy, sizeof(dummy));

                cur = timer_from_fd(pfd->fd);
                if (cur == NULL) {
                        continue;
                }

                /* Reset by client */
                if (!mask_test(&cur->state, BIT_ACTIVE)) {
                        continue;
                }

                timer_alarm(cur);
                mask_clear(&cur->state, BIT_ACTIVE);
        }
}

static void *worker(void *arg)
{
        int num_ready;
        size_t count;
        uint64_t dummy;
        unsigned int state;
        struct pollfd *pfd;
        struct pollfd pfds[gabs_array_size(timers)];

        (void)arg;

        for (;;) {
                pfd = pfds;
                pfd_push(&pfd, event_fd);

                state = atomic_load(&work_state);

                if (state == WORK_STATE_EXIT) {
                        break;
                }

                prepare_pfds(&pfd);
                count = pfd - pfds;

                num_ready = poll(pfds, count, -1);
                if (num_ready < 0) {
                        continue;
                }

                pfd = &pfds[0];
                if (pfd->revents & POLLIN) {
                        /* Reset set of fds being watched */
                        (void)read(pfd->fd, &dummy, sizeof(dummy));
                        continue;
                }

                service_pfds(pfd, count);
        }

        return NULL;
}

bool gabs_timer_okay(gabs_timer id)
{
        return timer_from_id(id) != NULL;
}

gabs_timer gabs_timer_install(gabs_timer_cb cb, void *user_data)
{
        return timer_to_id(timer_add(cb, user_data));
}

int gabs_timer_uninstall(gabs_timer id)
{
        (void)gabs_timer_stop(id);

        /* Perform cleanup when worker is done */
        mask_set(&timer_from_id(id)->state, BIT_ZOMBIE);

        return 0;
}

int gabs_timer_start(gabs_timer id, uint32_t delay_us)
{
        int status;
        struct timer_posix *t;

        t = timer_from_id(id);

        /* Already scheduled or running */
        if (mask_get(&t->state, BIT_ACTIVE | BIT_SCHED)) {
                return -EBUSY;
        }

        status = timer_restart(t, delay_us);

        return status;
}

int gabs_timer_restart(gabs_timer id, uint32_t delay_us)
{
        return timer_restart(timer_from_id(id), delay_us);
}

int gabs_timer_stop(gabs_timer id)
{
        int status;
        struct itimerspec spec;
        struct timer_posix *t;

        t = timer_from_id(id);

        mask_clear(&t->state, BIT_ACTIVE | BIT_SCHED);

        /* Reset timers so that we don't get a spurious event when the previous
         * time elapses. */
        spec = (struct itimerspec){0};
        status = timerfd_settime(t->fd, 0, &spec, NULL);
        if (status < 0) {
                status = -errno;
        }

        trigger_reset();

        return status;
}

bool gabs_timer_active(gabs_timer id)
{
        return mask_test(&timer_from_id(id)->state, BIT_ACTIVE);
}

POSIX_INIT(GABS_CONFIG_TIMER_INIT_PRIORITY)
{
        work_state = WORK_STATE_NORMAL;

        event_fd = eventfd(0, EFD_SEMAPHORE);
        if (event_fd < 0) {
                return;
        }

        (void)pthread_create(&thread_h, NULL, worker, NULL);
}

POSIX_DEINIT(GABS_CONFIG_TIMER_INIT_PRIORITY)
{
        atomic_store(&work_state, WORK_STATE_EXIT);
        trigger_reset();

        (void)pthread_join(thread_h, NULL);
        (void)close(event_fd);
}
