
#include <errno.h>

#include <gabs/testing/testing.h>

#include <gabs/semaphore.h>
#include <gabs/timer.h>

namespace
{

constexpr uint64_t long_delay_us = 60ull * 1000 * 1000;
constexpr uint64_t short_delay_us = 1000;
constexpr uint64_t stop_delay_us = 50 * 1000;
constexpr uint64_t fire_timeout_us = 2ull * 1000 * 1000;
constexpr uint64_t no_fire_timeout_us = 200 * 1000;

void noop_cb(gabs_timer, void *)
{
}

class ctx_fixture
{
      public:
        ctx_fixture()
        {
                GABS_ASSERT_EQ(gabs_timer_ctx_init(&ctx), 0);
        }

        ~ctx_fixture()
        {
                GABS_EXPECT_EQ(gabs_timer_ctx_deinit(&ctx), 0);
        }

        gabs_timer_ctx ctx;
};

struct fire_record {
        fire_record()
        {
                GABS_ASSERT_EQ(gabs_sem_init(&fired, 0), 0);
        }

        ~fire_record()
        {
                GABS_EXPECT_EQ(gabs_sem_deinit(&fired), 0);
        }

        gabs_sem fired;
        gabs_timer timer = {};
        void *arg = nullptr;
        int count = 0;
        int restarts = 0;
        int status = 0;
};

void record_cb(gabs_timer t, void *arg)
{
        auto record = static_cast<fire_record *>(arg);

        record->timer = t;
        record->arg = arg;
        record->count++;

        (void)gabs_sem_up(&record->fired);
}

void restart_cb(gabs_timer t, void *arg)
{
        auto record = static_cast<fire_record *>(arg);

        record->count++;

        if (record->restarts > 0) {
                record->restarts--;
                record->status = gabs_timer_restart(t, short_delay_us);
                return;
        }

        (void)gabs_sem_up(&record->fired);
}

void uninstall_cb(gabs_timer t, void *arg)
{
        auto record = static_cast<fire_record *>(arg);

        record->status = gabs_timer_uninstall(t);

        (void)gabs_sem_up(&record->fired);
}

} // namespace

GABS_SUITE(timer_lifecycle);

GABS_TEST(timer_lifecycle, init_deinit)
{
        gabs_timer_ctx ctx;

        GABS_ASSERT_EQ(gabs_timer_ctx_init(&ctx), 0);
        GABS_ASSERT_EQ(gabs_timer_ctx_deinit(&ctx), 0);
}

GABS_TEST(timer_lifecycle, deinit_with_active_timer)
{
        gabs_timer_ctx ctx;
        gabs_timer t;

        GABS_ASSERT_EQ(gabs_timer_ctx_init(&ctx), 0);

        t = gabs_timer_install(&ctx, noop_cb, nullptr);
        GABS_EXPECT_TRUE(gabs_timer_okay(t));
        GABS_EXPECT_EQ(gabs_timer_start(t, long_delay_us), 0);

        GABS_ASSERT_EQ(gabs_timer_ctx_deinit(&ctx), 0);
}

GABS_SUITE_F(timer, ctx_fixture);

GABS_TEST_F(timer, install_is_okay_and_idle)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_EXPECT_FALSE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, start_makes_active)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, long_delay_us), 0);
        GABS_EXPECT_TRUE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, start_while_active_is_busy)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, long_delay_us), 0);
        GABS_EXPECT_EQ(gabs_timer_start(t, long_delay_us), -EBUSY);

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, restart_while_idle_makes_active)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_restart(t, long_delay_us), 0);
        GABS_EXPECT_TRUE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, restart_while_active_keeps_active)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, long_delay_us), 0);
        GABS_EXPECT_EQ(gabs_timer_restart(t, long_delay_us), 0);
        GABS_EXPECT_TRUE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, stop_makes_idle)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, long_delay_us), 0);
        GABS_EXPECT_EQ(gabs_timer_stop(t), 0);
        GABS_EXPECT_FALSE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, fires_with_timer_and_arg)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, short_delay_us), 0);
        GABS_EXPECT_EQ(gabs_sem_down(&record.fired, fire_timeout_us), 0);
        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);

        GABS_ASSERT_EQ(record.count, 1);
        GABS_ASSERT_EQ(record.timer, t);
        GABS_ASSERT_EQ(record.arg, &record);
}

GABS_TEST_F(timer, fires_with_zero_delay)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, 0), 0);
        GABS_EXPECT_EQ(gabs_sem_down(&record.fired, fire_timeout_us), 0);
        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);

        GABS_ASSERT_EQ(record.count, 1);
}

GABS_TEST_F(timer, timers_fire_independently)
{
        fire_record first;
        fire_record second;
        gabs_timer a = gabs_timer_install(&fixture->ctx, record_cb, &first);
        gabs_timer b = gabs_timer_install(&fixture->ctx, record_cb, &second);
        GABS_ASSERT_TRUE(gabs_timer_okay(a));
        GABS_ASSERT_TRUE(gabs_timer_okay(b));

        GABS_EXPECT_EQ(gabs_timer_start(a, short_delay_us), 0);
        GABS_EXPECT_EQ(gabs_timer_start(b, short_delay_us), 0);
        GABS_EXPECT_EQ(gabs_sem_down(&first.fired, fire_timeout_us), 0);
        GABS_EXPECT_EQ(gabs_sem_down(&second.fired, fire_timeout_us), 0);
        GABS_EXPECT_EQ(gabs_timer_uninstall(a), 0);
        GABS_EXPECT_EQ(gabs_timer_uninstall(b), 0);

        GABS_ASSERT_EQ(first.timer, a);
        GABS_ASSERT_EQ(second.timer, b);
}

GABS_TEST_F(timer, stop_before_expiry_prevents_fire)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, stop_delay_us), 0);
        GABS_EXPECT_EQ(gabs_timer_stop(t), 0);
        GABS_EXPECT_NE(gabs_sem_down(&record.fired, no_fire_timeout_us), 0);
        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);

        GABS_ASSERT_EQ(record.count, 0);
}

GABS_TEST_F(timer, uninstall_before_expiry_prevents_fire)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, stop_delay_us), 0);
        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
        GABS_EXPECT_NE(gabs_sem_down(&record.fired, no_fire_timeout_us), 0);

        GABS_ASSERT_EQ(record.count, 0);
}

GABS_TEST_F(timer, restart_after_fire_fires_again)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, short_delay_us), 0);
        GABS_EXPECT_EQ(gabs_sem_down(&record.fired, fire_timeout_us), 0);
        GABS_EXPECT_EQ(gabs_timer_restart(t, short_delay_us), 0);
        GABS_EXPECT_EQ(gabs_sem_down(&record.fired, fire_timeout_us), 0);
        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);

        GABS_ASSERT_EQ(record.count, 2);
}

GABS_TEST_F(timer, restart_from_callback_fires_again)
{
        fire_record record;
        gabs_timer t;

        record.restarts = 2;
        t = gabs_timer_install(&fixture->ctx, restart_cb, &record);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_start(t, short_delay_us), 0);
        GABS_EXPECT_EQ(gabs_sem_down(&record.fired, fire_timeout_us), 0);
        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);

        GABS_ASSERT_EQ(record.count, 3);
        GABS_ASSERT_EQ(record.status, 0);
}

GABS_TEST_F(timer, uninstall_from_own_callback)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, uninstall_cb, &record);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        record.status = -1;
        GABS_EXPECT_EQ(gabs_timer_start(t, short_delay_us), 0);
        GABS_EXPECT_EQ(gabs_sem_down(&record.fired, fire_timeout_us), 0);

        GABS_ASSERT_EQ(record.status, 0);
}
