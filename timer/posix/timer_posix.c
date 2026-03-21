
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

#include <gabs/log.h>
#include <gabs/core/util.h>
#include <gabs/timer.h>

#include "atomic_mask.h"

#define BIT_USED   (1 << 0) /* Set by client, unset by worker */
#define BIT_ZOMBIE (1 << 1) /* Set by client, unset by worker */
#define BIT_ACTIVE (1 << 2) /* Set/unset by worker, unset by client */
#define BIT_SCHED  (1 << 3) /* Set by client, unset by worker */

GABS_LOGGER_DECLARE(logger, timer);

enum {
        WORK_STATE_NORMAL,
        WORK_STATE_STOPPED,
        WORK_STATE_EXIT,
};

static void trigger_reset(struct gabs_timer_posix_ctx *ctx)
{
        uint64_t count;
        int size;

        count = 1;

        size = write(ctx->eventfd, &count, sizeof(count));
        assert(size == sizeof(count));

        gabs_log_dbgf(logger, "Triggering reset");
}

static void to_itimerspec(struct itimerspec *spec, uint64_t time_us)
{
        spec->it_interval.tv_sec = 0;
        spec->it_interval.tv_nsec = 0;

        spec->it_value.tv_sec = time_us / (uint32_t)1e6;
        spec->it_value.tv_nsec = (time_us % (uint32_t)1e6) * 1000;

        if (spec->it_value.tv_sec == 0 && spec->it_value.tv_nsec == 0) {
                spec->it_value.tv_nsec = 1;
        }
}

static int timer_restart(struct gabs_timer_posix *t, uint64_t delay_us)
{
        int status;
        struct itimerspec spec;

        /* Make sure timer does not fire and/or start before we schedule
         * it again. */
        mask_clear(&t->state, BIT_ACTIVE | BIT_SCHED);

        to_itimerspec(&spec, delay_us);

        status = timerfd_settime(t->fd, 0, &spec, NULL);

        mask_set(&t->state, BIT_SCHED);
        trigger_reset(t->ctx);

        if (status < 0) {
                return -errno;
        }

        return status;
}

static bool timer_valid(struct gabs_timer_posix *t)
{
        return mask_get(&t->state, BIT_USED | BIT_ZOMBIE) == BIT_USED;
}

static void timer_cleanup(struct gabs_timer_posix *t)
{
        close(t->fd);

        /* Reset state to default */
        atomic_store(&t->state, 0);
}

static struct gabs_timer_posix *timer_from_fd(struct gabs_timer_posix_ctx *ctx,
                                              int fd)
{
        struct gabs_timer_posix *t;
        size_t i;

        for (i = 0; i < gabs_array_size(ctx->timers); i++) {
                t = &ctx->timers[i];

                if (!timer_valid(t)) {
                        continue;
                }

                if (t->fd == fd) {
                        return t;
                }
        }

        return NULL;
}

static struct gabs_timer_posix *timer_alloc(struct gabs_timer_posix_ctx *ctx)
{
        struct gabs_timer_posix *cur;
        size_t i;

        for (i = 0; i < gabs_array_size(ctx->timers); i++) {
                cur = &ctx->timers[i];

                if (mask_set_strict(&cur->state, BIT_USED)) {
                        return cur;
                }
        }

        return NULL;
}

static struct gabs_timer_posix *timer_add(struct gabs_timer_posix_ctx *ctx,
                                          gabs_timer_cb cb, void *user_data)
{
        struct gabs_timer_posix *t;

        t = timer_alloc(ctx);
        if (t == NULL) {
                return NULL;
        }

        /* Initial state. Safe to update here, will only go from used (with
         * potentially other bits) to only used set. */
        atomic_store(&t->state, BIT_USED);

        t->ctx = ctx;
        t->cb = cb;
        t->user_arg = user_data;
        t->fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

        if (t->fd < 0) {
                return NULL;
        }

        return t;
}

static void timer_alarm(struct gabs_timer_posix *t)
{
        if (t->cb == NULL) {
                return;
        }

        t->cb(t, t->user_arg);
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
static void prepare_pfds(struct gabs_timer_posix_ctx *ctx,
                         struct pollfd **pfdptr)
{
        size_t i;
        struct gabs_timer_posix *cur;

        for (i = 0; i < gabs_array_size(ctx->timers); i++) {
                cur = &ctx->timers[i];
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

static void service_pfds(struct gabs_timer_posix_ctx *ctx, struct pollfd *pfd,
                         size_t count)
{
        struct gabs_timer_posix *cur;
        size_t i;
        uint64_t dummy;

        for (i = 0; i < count; i++, pfd++) {
                if (!(pfd->revents & POLLIN)) {
                        continue;
                }

                gabs_log_dbgf(logger, "Timer alarm");
                (void)read(pfd->fd, &dummy, sizeof(dummy));

                cur = timer_from_fd(ctx, pfd->fd);
                if (cur == NULL) {
                        gabs_log_errf(logger, "Unrecognized timer fd: %i",
                                      pfd->fd);
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

static void *worker(void *ctx_arg)
{
        int num_ready;
        size_t count;
        uint64_t dummy;
        unsigned int state;
        struct gabs_timer_posix_ctx *ctx = ctx_arg;
        struct pollfd *pfd;
        struct pollfd pfds[gabs_array_size(ctx->timers)];

        for (;;) {
                pfd = pfds;
                pfd_push(&pfd, ctx->eventfd);

                state = atomic_load(&ctx->work_state);

                if (state == WORK_STATE_EXIT) {
                        gabs_log_inff(logger, "Exiting timer worker thread");
                        atomic_store(&ctx->stopped, true);
                        break;
                } else if (state == WORK_STATE_STOPPED) {
                        atomic_store(&ctx->stopped, true);
                } else {
                        atomic_store(&ctx->stopped, false);
                        prepare_pfds(ctx, &pfd);
                }

                count = pfd - pfds;

                num_ready = poll(pfds, count, -1);
                if (num_ready < 0) {
                        gabs_log_errf(logger, "Polling timers returned %i",
                                      errno);
                        continue;
                }

                pfd = &pfds[0];
                if (pfd->revents & POLLIN) {
                        /* Reset set of fds being watched */
                        gabs_log_dbgf(logger, "Timer thread reloaded");
                        (void)read(pfd->fd, &dummy, sizeof(dummy));
                        continue;
                }

                service_pfds(ctx, pfd, count);
        }

        return NULL;
}

int gabs_timer_ctx_init(struct gabs_timer_posix_ctx *ctx)
{
        int status;

        atomic_store(&ctx->work_state, WORK_STATE_STOPPED);
        atomic_store(&ctx->stopped, true);

        ctx->eventfd = eventfd(0, EFD_SEMAPHORE);
        if (ctx->eventfd < 0) {
                gabs_log_errf(logger, "Failed to create eventfd");
                return -errno;
        }

        status = pthread_create(&ctx->thread, NULL, worker, ctx);
        if (status != 0) {
                gabs_log_errf(logger, "Failed to create thread");
                (void)close(ctx->eventfd);
                status = -status;
        }

        return status;
}

int gabs_timer_ctx_deinit(struct gabs_timer_posix_ctx *ctx)
{
        gabs_log_dbgf(logger, "Destroying timer context");

        atomic_store(&ctx->work_state, WORK_STATE_EXIT);
        trigger_reset(ctx);

        (void)pthread_join(ctx->thread, NULL);
        (void)close(ctx->eventfd);

        return 0;
}

int gabs_timer_ctx_stop(struct gabs_timer_posix_ctx *ctx)
{
        unsigned int expected;

        expected = WORK_STATE_NORMAL;
        while (!atomic_compare_exchange_weak(&ctx->work_state, &expected,
                                             WORK_STATE_STOPPED)) {
                if (expected == WORK_STATE_STOPPED) {
                        return 0;
                }

                if (expected == WORK_STATE_EXIT) {
                        return -EINVAL;
                }
        }

        trigger_reset(ctx);

        /* TODO: This waiting could be improved with a futex */
        while (!atomic_load_explicit(&ctx->stopped, memory_order_relaxed)) {
        }

        return 0;
}

int gabs_timer_ctx_start(struct gabs_timer_posix_ctx *ctx)
{
        unsigned int expected;

        expected = WORK_STATE_STOPPED;

        while (!atomic_load_explicit(&ctx->stopped, memory_order_relaxed)) {
        }

        while (!atomic_compare_exchange_weak(&ctx->work_state, &expected,
                                             WORK_STATE_NORMAL)) {
                if (expected == WORK_STATE_NORMAL) {
                        break;
                }

                if (expected == WORK_STATE_EXIT) {
                        return -EINVAL;
                }
        }

        return 0;
}

bool gabs_timer_okay(struct gabs_timer_posix *t)
{
        return t != NULL;
}

gabs_timer gabs_timer_install(struct gabs_timer_posix_ctx *ctx,
                              gabs_timer_cb cb, void *user_data)
{
        return timer_add(ctx, cb, user_data);
}

int gabs_timer_uninstall(struct gabs_timer_posix *t)
{
        (void)gabs_timer_stop(t);

        /* Perform cleanup when worker is done */
        mask_set(&t->state, BIT_ZOMBIE);

        return 0;
}

int gabs_timer_start(struct gabs_timer_posix *t, uint64_t delay_us)
{
        int status;

        /* Already scheduled or running */
        if (mask_get(&t->state, BIT_ACTIVE | BIT_SCHED)) {
                return -EBUSY;
        }

        status = timer_restart(t, delay_us);

        return status;
}

int gabs_timer_restart(struct gabs_timer_posix *t, uint64_t delay_us)
{
        return timer_restart(t, delay_us);
}

int gabs_timer_stop(struct gabs_timer_posix *t)
{
        int status;
        struct itimerspec spec;

        mask_clear(&t->state, BIT_ACTIVE | BIT_SCHED);

        /* Reset timers so that we don't get a spurious event when the previous
         * time elapses. */
        spec = (struct itimerspec){0};
        status = timerfd_settime(t->fd, 0, &spec, NULL);
        if (status < 0) {
                status = -errno;
        }

        trigger_reset(t->ctx);

        return status;
}

bool gabs_timer_active(struct gabs_timer_posix *t)
{
        return mask_test(&t->state, BIT_ACTIVE);
}
