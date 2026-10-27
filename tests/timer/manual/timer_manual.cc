#include <errno.h>

#include <vector>

#include <gabs/testing/testing.h>

#include <gabs/timer.h>
#include <gabs/timer/manual.h>

namespace
{

constexpr uint64_t delay_us = 1000;

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
        const gabs_timer_ctx *ctx = nullptr;
        gabs_timer timer = {};
        void *arg = nullptr;
        int count = 0;
        int restarts = 0;
        int status = 0;
        std::vector<uint64_t> times;
        std::vector<int> *order = nullptr;
        int id = 0;
};

void record_cb(gabs_timer t, void *arg)
{
        auto record = static_cast<fire_record *>(arg);

        record->timer = t;
        record->arg = arg;
        record->count++;

        if (record->ctx != nullptr) {
                record->times.push_back(gabs_timer_manual_now(record->ctx));
        }

        if (record->order != nullptr) {
                record->order->push_back(record->id);
        }
}

void restart_cb(gabs_timer t, void *arg)
{
        auto record = static_cast<fire_record *>(arg);

        record_cb(t, arg);

        if (record->restarts > 0) {
                record->restarts--;
                record->status = gabs_timer_restart(t, delay_us);
        }
}

void uninstall_cb(gabs_timer t, void *arg)
{
        auto record = static_cast<fire_record *>(arg);

        record->count++;
        record->status = gabs_timer_uninstall(t);
}

} // namespace

GABS_SUITE_F(timer_manual, ctx_fixture);

GABS_TEST_F(timer_manual, clock_starts_at_zero)
{
        GABS_EXPECT_EQ(gabs_timer_manual_now(&fixture->ctx), 0u);
}

GABS_TEST_F(timer_manual, advance_moves_clock)
{
        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us), 0);
        GABS_EXPECT_EQ(gabs_timer_manual_now(&fixture->ctx), delay_us);
}

GABS_TEST_F(timer_manual, fires_at_expiry)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us - 1),
                       0);
        GABS_EXPECT_EQ(record.count, 0);
        GABS_EXPECT_TRUE(gabs_timer_active(t));

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, 1), 1);
        GABS_EXPECT_EQ(record.count, 1);
        GABS_EXPECT_EQ(record.timer, t);
        GABS_EXPECT_EQ(record.arg, static_cast<void *>(&record));
        GABS_EXPECT_FALSE(gabs_timer_active(t));

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, callback_sees_clock_at_expiry)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        record.ctx = &fixture->ctx;
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, 3 * delay_us),
                       1);
        GABS_ASSERT_EQ(record.times.size(), 1u);
        GABS_EXPECT_EQ(record.times[0], delay_us);
        GABS_EXPECT_EQ(gabs_timer_manual_now(&fixture->ctx), 3 * delay_us);

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, delay_counts_from_start)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        (void)gabs_timer_manual_advance(&fixture->ctx, delay_us);
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us - 1),
                       0);
        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, 1), 1);
        GABS_EXPECT_EQ(record.count, 1);

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, start_when_active_is_busy)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);
        GABS_EXPECT_EQ(gabs_timer_start(t, 2 * delay_us), -EBUSY);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us), 1);
        GABS_EXPECT_EQ(record.count, 1);

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, restart_resets_expiry)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);
        (void)gabs_timer_manual_advance(&fixture->ctx, delay_us / 2);
        GABS_ASSERT_EQ(gabs_timer_restart(t, delay_us), 0);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us - 1),
                       0);
        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, 1), 1);
        GABS_EXPECT_EQ(record.count, 1);

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, stop_prevents_firing)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);
        GABS_ASSERT_EQ(gabs_timer_stop(t), 0);

        GABS_EXPECT_FALSE(gabs_timer_active(t));
        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us), 0);
        GABS_EXPECT_EQ(record.count, 0);

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, uninstall_prevents_firing)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);
        GABS_ASSERT_EQ(gabs_timer_uninstall(t), 0);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us), 0);
        GABS_EXPECT_EQ(record.count, 0);
}

GABS_TEST_F(timer_manual, fires_in_order_of_expiry)
{
        std::vector<int> order;
        fire_record records[3];
        gabs_timer timers[3];
        const uint64_t delays[3] = {3 * delay_us, delay_us, 2 * delay_us};

        for (int i = 0; i < 3; i++) {
                records[i].order = &order;
                records[i].id = i;
                timers[i] = gabs_timer_install(&fixture->ctx, record_cb,
                                               &records[i]);
                GABS_ASSERT_TRUE(gabs_timer_okay(timers[i]));
                GABS_ASSERT_EQ(gabs_timer_start(timers[i], delays[i]), 0);
        }

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, 3 * delay_us),
                       3);
        GABS_EXPECT_TRUE((order == std::vector<int>{1, 2, 0}));

        for (gabs_timer t : timers) {
                GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
        }
}

GABS_TEST_F(timer_manual, equal_expiry_fires_in_start_order)
{
        std::vector<int> order;
        fire_record records[3];
        gabs_timer timers[3];

        for (int i = 0; i < 3; i++) {
                records[i].order = &order;
                records[i].id = i;
                timers[i] = gabs_timer_install(&fixture->ctx, record_cb,
                                               &records[i]);
                GABS_ASSERT_TRUE(gabs_timer_okay(timers[i]));
        }

        for (int i : {2, 0, 1}) {
                GABS_ASSERT_EQ(gabs_timer_start(timers[i], delay_us), 0);
        }

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us), 3);
        GABS_EXPECT_TRUE((order == std::vector<int>{2, 0, 1}));

        for (gabs_timer t : timers) {
                GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
        }
}

GABS_TEST_F(timer_manual, restart_from_callback_fires_within_advance)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, restart_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        record.ctx = &fixture->ctx;
        record.restarts = 5;
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, 3 * delay_us),
                       3);
        GABS_EXPECT_EQ(record.status, 0);
        GABS_EXPECT_TRUE(
                (record.times ==
                 std::vector<uint64_t>{delay_us, 2 * delay_us, 3 * delay_us}));
        GABS_EXPECT_TRUE(gabs_timer_active(t));

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, start_from_callback_succeeds)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(
                &fixture->ctx,
                [](gabs_timer t, void *arg) {
                        auto record = static_cast<fire_record *>(arg);

                        record->count++;
                        record->status = gabs_timer_start(t, delay_us);
                },
                &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us), 1);
        GABS_EXPECT_EQ(record.status, 0);
        GABS_EXPECT_TRUE(gabs_timer_active(t));

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, uninstall_from_callback)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, uninstall_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us), 1);
        GABS_EXPECT_EQ(record.count, 1);
        GABS_EXPECT_EQ(record.status, 0);
}

GABS_TEST_F(timer_manual, fire_runs_callback_without_moving_clock)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));
        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);

        GABS_EXPECT_EQ(gabs_timer_manual_fire(t), 0);
        GABS_EXPECT_EQ(record.count, 1);
        GABS_EXPECT_EQ(record.timer, t);
        GABS_EXPECT_FALSE(gabs_timer_active(t));
        GABS_EXPECT_EQ(gabs_timer_manual_now(&fixture->ctx), 0u);

        GABS_EXPECT_EQ(gabs_timer_manual_advance(&fixture->ctx, delay_us), 0);
        GABS_EXPECT_EQ(record.count, 1);

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, fire_inactive_is_invalid)
{
        fire_record record;
        gabs_timer t = gabs_timer_install(&fixture->ctx, record_cb, &record);

        GABS_ASSERT_TRUE(gabs_timer_okay(t));

        GABS_EXPECT_EQ(gabs_timer_manual_fire(t), -EINVAL);
        GABS_EXPECT_EQ(record.count, 0);

        GABS_ASSERT_EQ(gabs_timer_start(t, delay_us), 0);
        GABS_ASSERT_EQ(gabs_timer_stop(t), 0);
        GABS_EXPECT_EQ(gabs_timer_manual_fire(t), -EINVAL);
        GABS_EXPECT_EQ(record.count, 0);

        GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
}

GABS_TEST_F(timer_manual, install_fails_when_pool_is_full)
{
        fire_record record;
        gabs_timer timers[GABS_CONFIG_TIMER_MANUAL_MAX];

        for (gabs_timer &t : timers) {
                t = gabs_timer_install(&fixture->ctx, record_cb, &record);
                GABS_ASSERT_TRUE(gabs_timer_okay(t));
        }

        GABS_EXPECT_FALSE(gabs_timer_okay(
                gabs_timer_install(&fixture->ctx, record_cb, &record)));

        GABS_EXPECT_EQ(gabs_timer_uninstall(timers[0]), 0);
        timers[0] = gabs_timer_install(&fixture->ctx, record_cb, &record);
        GABS_EXPECT_TRUE(gabs_timer_okay(timers[0]));

        for (gabs_timer t : timers) {
                GABS_EXPECT_EQ(gabs_timer_uninstall(t), 0);
        }
}
