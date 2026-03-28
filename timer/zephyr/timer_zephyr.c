
#include <zephyr/kernel.h>
#include <zephyr/sys/dlist.h>
#include <zephyr/sys/bitarray.h>

#include <gabs/timer.h>

#ifndef CONFIG_GABS_TIMER_MAX
#define CONFIG_GABS_TIMER_MAX (20)
#endif /* CONFIG_GABS_TIMER_MAX */

struct timer_info {
        struct k_work_delayable dwork;

        gabs_timer_cb callback;
        void *user_data;

        sys_dnode_t dnode;
        struct gabs_timer_zephyr_ctx *ctx;
};

static struct timer_info pool[CONFIG_GABS_TIMER_MAX];
SYS_BITARRAY_DEFINE_STATIC(pool_bitarray, CONFIG_GABS_TIMER_MAX);

static __thread struct timer_info *firing = NULL;

static size_t to_idx(struct timer_info *info)
{
        __ASSERT_NO_MSG(info >= pool);
        return info - pool;
}

static struct timer_info *reserve(void)
{
        int status;
        size_t idx;

        status = sys_bitarray_alloc(&pool_bitarray, 1, &idx);
        __ASSERT(status == 0, "Timer allocation failed.");

        return &pool[idx];
}

static void release(struct timer_info *info)
{
        int status;

        status = sys_bitarray_free(&pool_bitarray, 1, to_idx(info));
        __ASSERT_NO_MSG(status == 0);
}

static void timer_expiry(struct k_work *work)
{
        struct timer_info *info;
        struct k_work_delayable *dwork;

        dwork = k_work_delayable_from_work(work);
        info = CONTAINER_OF(dwork, struct timer_info, dwork);

        firing = info;

        if (info->callback && (k_work_busy_get(work) & K_WORK_CANCELING) == 0) {
                info->callback(info, info->user_data);
        }

        firing = NULL;
}

bool gabs_timer_okay(gabs_timer timer_arg)
{
        return true;
}

gabs_timer gabs_timer_install(struct gabs_timer_zephyr_ctx *ctx,
                              gabs_timer_cb cb, void *user_data)
{
        struct timer_info *info;

        info = reserve();
        info->callback = cb;
        info->user_data = user_data;
        info->ctx = ctx;

        (void)k_mutex_lock(&ctx->lock, K_FOREVER);
        sys_dlist_append(&ctx->list, &info->dnode);
        (void)k_mutex_unlock(&ctx->lock);

        return info;
}

int gabs_timer_uninstall(gabs_timer timer)
{
        struct timer_info *info = timer;
        struct k_work_sync sync;

        (void)k_mutex_lock(&info->ctx->lock, K_FOREVER);
        sys_dlist_remove(&info->dnode);
        (void)k_mutex_unlock(&info->ctx->lock);

        /* If this is invokation originates from a work handler, we can't use
         * the sync canceller so we use the async one instead.
         *
         * This ensures that the work handler does not fire the user-specified
         * callback after this lock is released, which is a requirement
         * for preventing potential use-after-free. */
        if (info == firing) {
                (void)k_work_cancel_delayable(&info->dwork);
        } else {
                (void)k_work_cancel_delayable_sync(&info->dwork, &sync);
        }

        release(info);

        return 0;
}

static int timer_start(struct timer_info *info, uint64_t delay_us)
{
        int status;
        k_timeout_t timeout;

        if (delay_us == 0) {
                timeout = K_TICKS(1);
        } else {
                timeout = K_USEC(delay_us);
        }

        status = k_work_reschedule(&info->dwork, timeout);
        __ASSERT_NO_MSG(status >= 0);

        return 0;
}

int gabs_timer_start(gabs_timer timer, uint64_t delay_us)
{
        struct timer_info *info = timer;

        if (k_work_delayable_busy_get(&info->dwork) != 0) {
                return -EBUSY;
        }

        return timer_start(info, delay_us);
}

int gabs_timer_restart(gabs_timer timer, uint64_t delay_us)
{
        return timer_start(timer, delay_us);
}

int gabs_timer_stop(gabs_timer timer)
{
        struct timer_info *info = timer;

        (void)k_work_cancel_delayable(&info->dwork);

        return 0;
}

bool gabs_timer_active(gabs_timer timer)
{
        struct timer_info *info = timer;

        return k_work_delayable_busy_get(&info->dwork) &
               ~(K_WORK_CANCELING | K_WORK_FLUSHING);
}

int gabs_timer_ctx_init(struct gabs_timer_zephyr_ctx *ctx)
{
        sys_dlist_init(&ctx->list);
        k_mutex_init(&ctx->lock);

        return 0;
}

int gabs_timer_ctx_deinit(struct gabs_timer_zephyr_ctx *ctx)
{
        struct timer_info *info;
        struct timer_info *nxt;
        struct k_work_sync sync;

        SYS_DLIST_FOR_EACH_CONTAINER_SAFE(&ctx->list, info, nxt, dnode)
        {
                sys_dlist_remove(&info->dnode);

                (void)k_work_cancel_delayable_sync(&info->dwork, &sync);
                release(info);
        }

        return 0;
}

/* Pre-initialize timers. This prevents re-initing work item between
 * release/reserve, which could cause a problem if the work item is still being
 * canceled. */
static int init_timers(void)
{
        size_t i;
        struct timer_info *cur;

        for (i = 0; i < ARRAY_SIZE(pool); i++) {
                cur = &pool[i];

                k_work_init_delayable(&cur->dwork, timer_expiry);
        }

        return 0;
}

SYS_INIT(init_timers, POST_KERNEL, 0);
