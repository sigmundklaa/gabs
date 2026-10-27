
#include <gabs/testing/async.hh>
#include <gabs/testing/testing.h>

namespace
{

int constructed;
int destructed;

class counting_fixture
{
      public:
        static constexpr int initial_value = 1;

        counting_fixture() : value(initial_value)
        {
                constructed++;
        }

        ~counting_fixture()
        {
                destructed++;
        }

        int value;
};

} // namespace

GABS_SUITE(testing);

GABS_TEST(testing, passing_assertions_continue)
{
        int value = 0;

        GABS_ASSERT_TRUE(true);
        GABS_ASSERT_FALSE(false);
        GABS_ASSERT_EQ(1, 1);
        GABS_ASSERT_NE(1, 2);
        GABS_ASSERT_NULL(nullptr);
        GABS_ASSERT_NOT_NULL(&value);

        GABS_EXPECT_TRUE(true);
        GABS_EXPECT_FALSE(false);
        GABS_EXPECT_EQ(1, 1);
        GABS_EXPECT_NE(1, 2);
        GABS_EXPECT_NULL(nullptr);
        GABS_EXPECT_NOT_NULL(&value);
}

GABS_TEST(testing, skip_ends_case)
{
        GABS_TEST_SKIP();
        GABS_TEST_FAIL();
}

GABS_TEST(testing, async_get_returns_result)
{
        auto task = gabs::testing::async([] { return 42; });

        GABS_ASSERT_EQ(task.get(), 42);
}

GABS_TEST(testing, async_runs_before_get_returns)
{
        int value = 0;
        auto task = gabs::testing::async([&] { value = 42; });

        task.get();
        GABS_ASSERT_EQ(value, 42);
}

GABS_TEST(testing, async_runs_before_destruction_returns)
{
        int value = 0;

        {
                auto task = gabs::testing::async([&] { value = 42; });
        }

        GABS_ASSERT_EQ(value, 42);
}

GABS_SUITE_F(testing_fixture, counting_fixture);

GABS_TEST_F(testing_fixture, is_constructed_once_per_case)
{
        GABS_ASSERT_EQ(constructed, destructed + 1);
}

GABS_TEST_F(testing_fixture, is_fresh_in_first_case)
{
        GABS_ASSERT_EQ(fixture->value, counting_fixture::initial_value);
        fixture->value++;
}

GABS_TEST_F(testing_fixture, is_fresh_in_second_case)
{
        GABS_ASSERT_EQ(fixture->value, counting_fixture::initial_value);
        fixture->value++;
}

GABS_TEST(testing_fixture, plain_case_runs_with_fixture)
{
        GABS_ASSERT_EQ(constructed, destructed + 1);
}
