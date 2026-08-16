
#include <errno.h>

#include <gabs/testing/testing.h>

#include <gabs/timer.h>
#include <gabs/semaphore.h>

namespace
{

/* A timer that never fires during a test's synchronous checks, but is still
 * short enough that gabs_timer_ctx_deinit stopping it doesn't block the
 * suite if the stop is for some reason not instantaneous. */
constexpr uint64_t long_delay_us = 60ull * 1000 * 1000;

/* Generous relative to any delay used to actually let a timer fire, so a
 * failure to fire is reported as a failed wait rather than a hang. */
constexpr uint64_t fire_wait_timeout_us = 2ull * 1000 * 1000;

void noop_cb(gabs_timer, void *)
{
}

class timer_ctx_fixture
{
      public:
        timer_ctx_fixture()
        {
                GABS_ASSERT_EQ(gabs_timer_ctx_init(&ctx), 0);
        }

        /* `EXPECT` rather than `ASSERT`: a failing `ASSERT` may unwind by
         * throwing, which would terminate from a destructor. */
        ~timer_ctx_fixture()
        {
                GABS_EXPECT_EQ(gabs_timer_ctx_deinit(&ctx), 0);
        }

        gabs_timer_ctx ctx;
};

} // namespace

GABS_SUITE(timer_lifecycle);

GABS_TEST(timer_lifecycle, init_deinit)
{
        gabs_timer_ctx ctx;

        GABS_ASSERT_EQ(gabs_timer_ctx_init(&ctx), 0);
        GABS_ASSERT_EQ(gabs_timer_ctx_deinit(&ctx), 0);
}

GABS_TEST(timer_lifecycle, deinit_stops_active_timer)
{
        gabs_timer_ctx ctx;
        gabs_timer t;

        GABS_ASSERT_EQ(gabs_timer_ctx_init(&ctx), 0);

        t = gabs_timer_install(&ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_EQ(gabs_timer_start(t, long_delay_us), 0);

        GABS_ASSERT_EQ(gabs_timer_ctx_deinit(&ctx), 0);
}

GABS_SUITE_F(timer, timer_ctx_fixture);

GABS_TEST_F(timer, install_produces_okay_idle_timer)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_FALSE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, start_makes_timer_active)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_ASSERT_EQ(gabs_timer_start(t, long_delay_us), 0);
        GABS_ASSERT_TRUE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, start_while_active_fails)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_ASSERT_EQ(gabs_timer_start(t, long_delay_us), 0);
        GABS_ASSERT_EQ(gabs_timer_start(t, long_delay_us), -EBUSY);
        GABS_ASSERT_TRUE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, restart_while_idle_starts_timer)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_ASSERT_EQ(gabs_timer_restart(t, long_delay_us), 0);
        GABS_ASSERT_TRUE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, restart_while_active_still_succeeds)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_ASSERT_EQ(gabs_timer_start(t, long_delay_us), 0);
        GABS_ASSERT_EQ(gabs_timer_restart(t, long_delay_us), 0);
        GABS_ASSERT_TRUE(gabs_timer_active(t));

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer, stop_returns_success)
{
        gabs_timer t = gabs_timer_install(&fixture->ctx, noop_cb, nullptr);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_ASSERT_EQ(gabs_timer_start(t, long_delay_us), 0);
        GABS_ASSERT_EQ(gabs_timer_stop(t), 0);

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
}

namespace
{

struct fire_capture {
        gabs_sem sem;
        gabs_timer timer;
        void *user_arg;
};

void capture_fire_cb(gabs_timer t, void *arg)
{
        fire_capture *capture = static_cast<fire_capture *>(arg);

        capture->timer = t;
        capture->user_arg = arg;

        (void)gabs_sem_up(&capture->sem);
}

} // namespace

GABS_TEST_F(timer, callback_fires_with_installed_timer_and_user_arg)
{
        fire_capture capture = {};

        GABS_ASSERT_EQ(gabs_sem_init(&capture.sem, 0), 0);

        gabs_timer t = gabs_timer_install(&fixture->ctx, capture_fire_cb, &capture);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_ASSERT_EQ(gabs_timer_start(t, 1000), 0);
        GABS_ASSERT_EQ(gabs_sem_down(&capture.sem, fire_wait_timeout_us), 0);

        GABS_ASSERT_EQ(capture.timer, t);
        GABS_ASSERT_EQ(capture.user_arg, &capture);

        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);
        GABS_EXPECT_EQ(gabs_sem_deinit(&capture.sem), 0);
}

namespace
{

struct self_uninstall_capture {
        gabs_sem sem;
        int uninstall_status;
};

void self_uninstall_cb(gabs_timer t, void *arg)
{
        self_uninstall_capture *capture = static_cast<self_uninstall_capture *>(arg);

        capture->uninstall_status = gabs_timer_uninstall(t);

        (void)gabs_sem_up(&capture->sem);
}

} // namespace

GABS_TEST_F(timer, uninstall_from_within_own_callback_succeeds)
{
        self_uninstall_capture capture = {};

        GABS_ASSERT_EQ(gabs_sem_init(&capture.sem, 0), 0);

        gabs_timer t = gabs_timer_install(&fixture->ctx, self_uninstall_cb, &capture);
        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_ASSERT_EQ(gabs_timer_start(t, 1000), 0);
        GABS_ASSERT_EQ(gabs_sem_down(&capture.sem, fire_wait_timeout_us), 0);

        GABS_ASSERT_EQ(capture.uninstall_status, 0);

        GABS_EXPECT_EQ(gabs_sem_deinit(&capture.sem), 0);
}
