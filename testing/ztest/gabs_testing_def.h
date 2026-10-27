
#ifndef GABS_TESTING_ZTEST_DEF_H__
#define GABS_TESTING_ZTEST_DEF_H__

#ifndef GABS_TESTING_INCLUDE_DEF
#error "gabs_testing_def.h should not be included directly"
#endif /* GABS_TESTING_INCLUDE_DEF */

#include <new>

#include <zephyr/ztest.h>

#define GABS_SUITE(suite_) ZTEST_SUITE(suite_, NULL, NULL, NULL, NULL, NULL)

/* `ZTEST_F` requires the fixture type to be `struct <suite>_fixture`. */
#define GABS_SUITE_F(suite_, fixture_)                                         \
        struct suite_##_fixture : fixture_ {                                   \
        };                                                                     \
        alignas(struct suite_##_fixture) static unsigned char                  \
                gabs_testing_ztest_mem__##suite_[sizeof(                       \
                        struct suite_##_fixture)];                             \
        static void *gabs_testing_ztest_setup__##suite_(void)                  \
        {                                                                      \
                return gabs_testing_ztest_mem__##suite_;                       \
        }                                                                      \
        static void gabs_testing_ztest_before__##suite_(void *mem_)            \
        {                                                                      \
                new (mem_) suite_##_fixture();                                 \
        }                                                                      \
        static void gabs_testing_ztest_after__##suite_(void *mem_)             \
        {                                                                      \
                static_cast<suite_##_fixture *>(mem_)->~suite_##_fixture();    \
        }                                                                      \
        ZTEST_SUITE(suite_, NULL, gabs_testing_ztest_setup__##suite_,          \
                    gabs_testing_ztest_before__##suite_,                       \
                    gabs_testing_ztest_after__##suite_, NULL)

#define GABS_TEST(suite_, name_)   ZTEST(suite_, name_)
#define GABS_TEST_F(suite_, name_) ZTEST_F(suite_, name_)

#define GABS_ASSERT_TRUE(expr_)    zassert_true(expr_)
#define GABS_ASSERT_FALSE(expr_)   zassert_false(expr_)
#define GABS_ASSERT_EQ(a_, b_)     zassert_equal(a_, b_)
#define GABS_ASSERT_NE(a_, b_)     zassert_not_equal(a_, b_)
#define GABS_ASSERT_NULL(ptr_)     zassert_is_null(ptr_)
#define GABS_ASSERT_NOT_NULL(ptr_) zassert_not_null(ptr_)

#define GABS_EXPECT_TRUE(expr_)    zexpect_true(expr_)
#define GABS_EXPECT_FALSE(expr_)   zexpect_false(expr_)
#define GABS_EXPECT_EQ(a_, b_)     zexpect_equal(a_, b_)
#define GABS_EXPECT_NE(a_, b_)     zexpect_not_equal(a_, b_)
#define GABS_EXPECT_NULL(ptr_)     zexpect_is_null(ptr_)
#define GABS_EXPECT_NOT_NULL(ptr_) zexpect_not_null(ptr_)

#define GABS_TEST_FAIL() zassert_unreachable()
#define GABS_TEST_SKIP() ztest_test_skip()

#endif /* GABS_TESTING_ZTEST_DEF_H__ */
