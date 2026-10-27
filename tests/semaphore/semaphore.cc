
#include <gabs/testing/async.hh>
#include <gabs/testing/testing.h>

#include <gabs/common.h>
#include <gabs/semaphore.h>

namespace
{

constexpr uint64_t timeout_us = 10 * 1000;

class sem_fixture
{
      public:
        sem_fixture()
        {
                GABS_ASSERT_EQ(gabs_sem_init(&sem, 0), 0);
        }

        ~sem_fixture()
        {
                GABS_EXPECT_EQ(gabs_sem_deinit(&sem), 0);
        }

        gabs_sem sem;
};

} // namespace

GABS_SUITE(semaphore_lifecycle);

GABS_TEST(semaphore_lifecycle, init_deinit)
{
        gabs_sem sem;

        GABS_ASSERT_EQ(gabs_sem_init(&sem, 0), 0);
        GABS_ASSERT_EQ(gabs_sem_deinit(&sem), 0);
}

GABS_TEST(semaphore_lifecycle, init_sets_initial_count)
{
        gabs_sem sem;
        int first;
        int second;
        int third;

        GABS_ASSERT_EQ(gabs_sem_init(&sem, 2), 0);

        first = gabs_sem_down(&sem, GABS_TIMEOUT_NONE);
        second = gabs_sem_down(&sem, GABS_TIMEOUT_NONE);
        third = gabs_sem_down(&sem, GABS_TIMEOUT_NONE);
        GABS_ASSERT_EQ(gabs_sem_deinit(&sem), 0);

        GABS_ASSERT_EQ(first, 0);
        GABS_ASSERT_EQ(second, 0);
        GABS_ASSERT_NE(third, 0);
}

GABS_SUITE_F(semaphore, sem_fixture);

GABS_TEST_F(semaphore, down_no_wait_fails_when_empty)
{
        GABS_ASSERT_NE(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
}

GABS_TEST_F(semaphore, down_timeout_expires_when_empty)
{
        GABS_ASSERT_NE(gabs_sem_down(&fixture->sem, timeout_us), 0);
}

GABS_TEST_F(semaphore, down_no_wait_after_up)
{
        GABS_ASSERT_EQ(gabs_sem_up(&fixture->sem), 0);
        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
}

GABS_TEST_F(semaphore, down_timeout_after_up)
{
        GABS_ASSERT_EQ(gabs_sem_up(&fixture->sem), 0);
        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, timeout_us), 0);
}

GABS_TEST_F(semaphore, down_forever_after_up)
{
        GABS_ASSERT_EQ(gabs_sem_up(&fixture->sem), 0);
        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_MAX), 0);
}

GABS_TEST_F(semaphore, count_accumulates)
{
        GABS_ASSERT_EQ(gabs_sem_up(&fixture->sem), 0);
        GABS_ASSERT_EQ(gabs_sem_up(&fixture->sem), 0);

        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
        GABS_ASSERT_NE(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
}

GABS_TEST_F(semaphore, down_forever_returns_after_up_in_other_thread)
{
        auto up = gabs::testing::async([&] {
                return gabs_sem_up(&fixture->sem);
        });

        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_MAX), 0);
        GABS_ASSERT_EQ(up.get(), 0);
}

GABS_TEST_F(semaphore, up_wakes_down_forever_in_other_thread)
{
        auto down = gabs::testing::async([&] {
                return gabs_sem_down(&fixture->sem, GABS_TIMEOUT_MAX);
        });

        GABS_EXPECT_EQ(gabs_sem_up(&fixture->sem), 0);
        GABS_ASSERT_EQ(down.get(), 0);
}
