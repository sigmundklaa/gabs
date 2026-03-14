
#include <zephyr/kernel.h>
#include <zephyr/sys/bitarray.h>

#include <gabs/timer.h>

#ifndef CONFIG_GABS_TIMER_MAX
#define CONFIG_GABS_TIMER_MAX (20)
#endif /* CONFIG_GABS_TIMER_MAX */

struct timer_info {
        struct k_work_delayable dwork;
        struct k_work_sync sync;

        gabs_timer_cb callback;
        void *user_data;
};

static struct timer_info pool[CONFIG_GABS_TIMER_MAX];
SYS_BITARRAY_DEFINE_STATIC(pool_bitarray, CONFIG_GABS_TIMER_MAX);

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

        if (info->callback) {
                info->callback(info, info->user_data);
        }
}

bool gabs_timer_okay(gabs_timer timer_arg)
{
        return true;
}

gabs_timer gabs_timer_install(gabs_timer_cb cb, void *user_data)
{
        struct timer_info *info;

        info = reserve();
        info->callback = cb;
        info->user_data = user_data;

        k_work_init_delayable(&info->dwork, timer_expiry);

        return info;
}

int gabs_timer_uninstall(gabs_timer timer)
{
        struct timer_info *info = timer;

        (void)k_work_cancel_delayable_sync(&info->dwork, &info->sync);
        release(info);

        return 0;
}

static int timer_start(struct timer_info *info, uint32_t delay_us)
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

int gabs_timer_start(gabs_timer timer, uint32_t delay_us)
{
        struct timer_info *info = timer;

        if (k_work_delayable_busy_get(&info->dwork) != 0) {
                return -EBUSY;
        }

        return timer_start(info, delay_us);
}

int gabs_timer_restart(gabs_timer timer, uint32_t delay_us)
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

        return k_work_delayable_busy_get(&info->dwork);
}
