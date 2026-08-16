
#ifndef GABS_TESTING_CATCH2_DEF_H__
#define GABS_TESTING_CATCH2_DEF_H__

#ifndef GABS_TESTING_INCLUDE_DEF
#error "gabs_testing_def.h should not be included directly"
#endif /* GABS_TESTING_INCLUDE_DEF */

#include <catch2/catch_test_macros.hpp>

#define GABS_SUITE_F(suite_, fixture_)                                         \
        struct gabs_test_catch2_fixture__##suite_ : fixture_ {                 \
        }

#define GABS_SUITE(suite_)                                                     \
        struct gabs_test_catch2_fixture__##suite_ {                            \
        }

#define GABS_TEST(suite_, name_)                                               \
        TEST_CASE_METHOD(gabs_test_catch2_fixture__##suite_,                   \
                         #suite_ "::" #name_, "[" #suite_ "]")

/* `fixture` is marked as possibly unused, since a case of a fixtured suite
 * is permitted to ignore it. */
#define GABS_TEST_F(suite_, name_)                                             \
        static void gabs_test_catch2_body__##suite_##_##name_(                 \
                struct gabs_test_catch2_fixture__##suite_ *fixture);           \
        TEST_CASE_METHOD(gabs_test_catch2_fixture__##suite_,                   \
                         #suite_ "::" #name_, "[" #suite_ "]")                 \
        {                                                                      \
                gabs_test_catch2_body__##suite_##_##name_(this);               \
        }                                                                      \
        static void gabs_test_catch2_body__##suite_##_##name_(                 \
                [[maybe_unused]] struct gabs_test_catch2_fixture__##suite_     \
                        *fixture)

/* Operands are deliberately not wrapped in an outer set of parentheses:
 * `REQUIRE((expr))` suppresses the expression decomposition that supplies
 * the failure message. */
#define GABS_ASSERT_TRUE(expr_)    REQUIRE(expr_)
#define GABS_ASSERT_FALSE(expr_)   REQUIRE_FALSE(expr_)
#define GABS_ASSERT_EQ(a_, b_)     REQUIRE((a_) == (b_))
#define GABS_ASSERT_NE(a_, b_)     REQUIRE((a_) != (b_))
#define GABS_ASSERT_NULL(ptr_)     REQUIRE((ptr_) == nullptr)
#define GABS_ASSERT_NOT_NULL(ptr_) REQUIRE((ptr_) != nullptr)

#define GABS_EXPECT_TRUE(expr_)    CHECK(expr_)
#define GABS_EXPECT_FALSE(expr_)   CHECK_FALSE(expr_)
#define GABS_EXPECT_EQ(a_, b_)     CHECK((a_) == (b_))
#define GABS_EXPECT_NE(a_, b_)     CHECK((a_) != (b_))
#define GABS_EXPECT_NULL(ptr_)     CHECK((ptr_) == nullptr)
#define GABS_EXPECT_NOT_NULL(ptr_) CHECK((ptr_) != nullptr)

/* No message is accepted from the caller, so one is synthesized here from
 * the call site instead. */
#define GABS_TEST_FAIL() FAIL("explicit failure at " __FILE__ ":" << __LINE__)
#define GABS_TEST_SKIP() SKIP("skipped at " __FILE__ ":" << __LINE__)

#endif /* GABS_TESTING_CATCH2_DEF_H__ */
