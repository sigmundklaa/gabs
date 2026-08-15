
#include <gabs/testing/testing.h>

#include <gabs/common.h>
#include <gabs/mutex.h>

/*
 * Note that relocking from the owning thread is deliberately left untested:
 * the POSIX implementation reports an error, while the Zephyr one permits it
 * recursively. The gabs API does not specify either behaviour, so tests here
 * are restricted to what both implementations agree on.
 */

namespace
{

constexpr uint64_t finite_timeout = 1000;

class mutex_fixture
{
      public:
        mutex_fixture()
        {
                GABS_ASSERT_EQ(gabs_mutex_init(&lock), 0);
        }

        /* `EXPECT` rather than `ASSERT`: a failing `ASSERT` may unwind by
         * throwing, which would terminate from a destructor. */
        ~mutex_fixture()
        {
                GABS_EXPECT_EQ(gabs_mutex_deinit(&lock), 0);
        }

        gabs_mutex lock;
};

} // namespace

GABS_SUITE_F(mutex, mutex_fixture);

GABS_TEST_F(mutex, lock_forever_then_unlock)
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
        GABS_ASSERT_EQ(gabs_mutex_lock(&fixture->lock, finite_timeout), 0);
        GABS_ASSERT_EQ(gabs_mutex_unlock(&fixture->lock), 0);
}

GABS_TEST_F(mutex, relockable_after_unlock)
{
        for (int i = 0; i < 3; i++) {
                GABS_ASSERT_EQ(
                        gabs_mutex_lock(&fixture->lock, GABS_TIMEOUT_MAX), 0);
                GABS_ASSERT_EQ(gabs_mutex_unlock(&fixture->lock), 0);
        }
}

GABS_SUITE(mutex_lifecycle);

GABS_TEST(mutex_lifecycle, init_deinit)
{
        gabs_mutex lock;

        GABS_ASSERT_EQ(gabs_mutex_init(&lock), 0);
        GABS_ASSERT_EQ(gabs_mutex_deinit(&lock), 0);
}
