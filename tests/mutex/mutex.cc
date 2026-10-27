
#include <gabs/testing/async.hh>
#include <gabs/testing/testing.h>

#include <gabs/common.h>
#include <gabs/mutex.h>

namespace
{

constexpr uint64_t timeout_us = 10 * 1000;

class lock_fixture
{
      public:
        lock_fixture()
        {
                GABS_ASSERT_EQ(gabs_mutex_init(&lock), 0);
        }

        ~lock_fixture()
        {
                GABS_EXPECT_EQ(gabs_mutex_deinit(&lock), 0);
        }

        gabs_mutex lock;
};

int lock_in_other_thread(gabs_mutex *lock, uint64_t timeout_us)
{
        auto task = gabs::testing::async([=] {
                int status = gabs_mutex_lock(lock, timeout_us);

                if (status == 0) {
                        (void)gabs_mutex_unlock(lock);
                }

                return status;
        });

        return task.get();
}

} // namespace

GABS_SUITE(mutex_lifecycle);

GABS_TEST(mutex_lifecycle, init_deinit)
{
        gabs_mutex lock;

        GABS_ASSERT_EQ(gabs_mutex_init(&lock), 0);
        GABS_ASSERT_EQ(gabs_mutex_deinit(&lock), 0);
}

GABS_SUITE_F(mutex, lock_fixture);

GABS_TEST_F(mutex, lock_forever_when_free)
{
        GABS_ASSERT_EQ(gabs_mutex_lock(&fixture->lock, GABS_TIMEOUT_MAX), 0);
        GABS_ASSERT_EQ(gabs_mutex_unlock(&fixture->lock), 0);
}

GABS_TEST_F(mutex, lock_no_wait_when_free)
{
        GABS_ASSERT_EQ(gabs_mutex_lock(&fixture->lock, GABS_TIMEOUT_NONE), 0);
        GABS_ASSERT_EQ(gabs_mutex_unlock(&fixture->lock), 0);
}

GABS_TEST_F(mutex, lock_timeout_when_free)
{
        GABS_ASSERT_EQ(gabs_mutex_lock(&fixture->lock, timeout_us), 0);
        GABS_ASSERT_EQ(gabs_mutex_unlock(&fixture->lock), 0);
}

GABS_TEST_F(mutex, relock_after_unlock)
{
        for (int i = 0; i < 3; i++) {
                GABS_ASSERT_EQ(
                        gabs_mutex_lock(&fixture->lock, GABS_TIMEOUT_MAX), 0);
                GABS_ASSERT_EQ(gabs_mutex_unlock(&fixture->lock), 0);
        }
}

GABS_TEST_F(mutex, lock_no_wait_fails_when_held_by_other_thread)
{
        int status;

        GABS_ASSERT_EQ(gabs_mutex_lock(&fixture->lock, GABS_TIMEOUT_MAX), 0);
        status = lock_in_other_thread(&fixture->lock, GABS_TIMEOUT_NONE);
        GABS_ASSERT_EQ(gabs_mutex_unlock(&fixture->lock), 0);

        GABS_ASSERT_NE(status, 0);
}

GABS_TEST_F(mutex, lock_timeout_expires_when_held_by_other_thread)
{
        int status;

        GABS_ASSERT_EQ(gabs_mutex_lock(&fixture->lock, GABS_TIMEOUT_MAX), 0);
        status = lock_in_other_thread(&fixture->lock, timeout_us);
        GABS_ASSERT_EQ(gabs_mutex_unlock(&fixture->lock), 0);

        GABS_ASSERT_NE(status, 0);
}

GABS_TEST_F(mutex, lock_succeeds_in_other_thread_after_unlock)
{
        GABS_ASSERT_EQ(gabs_mutex_lock(&fixture->lock, GABS_TIMEOUT_MAX), 0);
        GABS_ASSERT_EQ(gabs_mutex_unlock(&fixture->lock), 0);

        GABS_ASSERT_EQ(lock_in_other_thread(&fixture->lock, GABS_TIMEOUT_NONE),
                       0);
}
