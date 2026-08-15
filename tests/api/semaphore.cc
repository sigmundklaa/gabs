
#include <gabs/testing/testing.h>

#include <gabs/common.h>
#include <gabs/semaphore.h>

namespace
{

/* Timeout used where the wait is expected to expire. Kept short, as nothing
 * ever gives the semaphore in these cases. */
constexpr uint64_t expiring_timeout = 1000;

class sem_fixture
{
      public:
        sem_fixture()
        {
                GABS_ASSERT_EQ(gabs_sem_init(&sem, 0), 0);
        }

        /* `EXPECT` rather than `ASSERT`: a failing `ASSERT` may unwind by
         * throwing, which would terminate from a destructor. */
        ~sem_fixture()
        {
                GABS_EXPECT_EQ(gabs_sem_deinit(&sem), 0);
        }

        gabs_sem sem;
};

} // namespace

GABS_SUITE_F(semaphore, sem_fixture);

GABS_TEST_F(semaphore, down_no_wait_when_empty_fails)
{
        GABS_ASSERT_NE(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
}

GABS_TEST_F(semaphore, down_no_wait_after_up)
{
        GABS_ASSERT_EQ(gabs_sem_up(&fixture->sem), 0);
        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
}

GABS_TEST_F(semaphore, down_forever_after_up)
{
        GABS_ASSERT_EQ(gabs_sem_up(&fixture->sem), 0);
        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_MAX), 0);
}

GABS_TEST_F(semaphore, down_timeout_expires_when_empty)
{
        GABS_ASSERT_NE(gabs_sem_down(&fixture->sem, expiring_timeout), 0);
}

GABS_TEST_F(semaphore, count_accumulates)
{
        GABS_ASSERT_EQ(gabs_sem_up(&fixture->sem), 0);
        GABS_ASSERT_EQ(gabs_sem_up(&fixture->sem), 0);

        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
        GABS_ASSERT_EQ(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);

        GABS_ASSERT_NE(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
}

GABS_TEST_F(semaphore, fixture_is_fresh_per_case)
{
        /* Would fail if the fixture leaked the count left over by
         * `count_accumulates`. */
        GABS_ASSERT_NE(gabs_sem_down(&fixture->sem, GABS_TIMEOUT_NONE), 0);
}
