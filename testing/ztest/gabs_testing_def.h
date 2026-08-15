
#ifndef GABS_TESTING_ZTEST_DEF_H__
#define GABS_TESTING_ZTEST_DEF_H__

#ifndef GABS_TESTING_INCLUDE_DEF
#error "gabs_testing_def.h should not be included directly"
#endif /* GABS_TESTING_INCLUDE_DEF */

#include <new>

#include <zephyr/ztest.h>

#define GABS_SUITE(suite_) ZTEST_SUITE(suite_, NULL, NULL, NULL, NULL, NULL)

/* `ZTEST_F` requires the fixture to be named `struct <suite>_fixture`, so the
 * user fixture is inherited rather than aliased. It is constructed in
 * `before` rather than `setup` so that each case gets a fresh one, matching
 * Catch2. Placement new into static storage keeps it off the heap, which may
 * not exist at all on the target. */
#define GABS_SUITE_F(suite_, fixture_)                                         \
        struct suite_##_fixture : fixture_ {                                   \
        };                                                                     \
        using gabs_test_ztest_fixture__##suite_ = struct suite_##_fixture;     \
        alignas(gabs_test_ztest_fixture__##suite_) static unsigned char        \
                gabs_test_ztest_mem__##suite_[sizeof(                          \
                        gabs_test_ztest_fixture__##suite_)];                   \
        static void *gabs_test_ztest_setup__##suite_(void)                     \
        {                                                                      \
                return gabs_test_ztest_mem__##suite_;                          \
        }                                                                      \
        static void gabs_test_ztest_before__##suite_(void *mem_)               \
        {                                                                      \
                new (mem_) gabs_test_ztest_fixture__##suite_();                \
        }                                                                      \
        static void gabs_test_ztest_after__##suite_(void *mem_)                \
        {                                                                      \
                static_cast<gabs_test_ztest_fixture__##suite_ *>(mem_)         \
                        ->~gabs_test_ztest_fixture__##suite_();                \
        }                                                                      \
        ZTEST_SUITE(suite_, NULL, gabs_test_ztest_setup__##suite_,             \
                    gabs_test_ztest_before__##suite_,                          \
                    gabs_test_ztest_after__##suite_, NULL)

#define GABS_TEST(suite_, name_)   ZTEST(suite_, name_)
#define GABS_TEST_F(suite_, name_) ZTEST_F(suite_, name_)

/* Operands are passed through unmodified so that the stringified default
 * message of ztest (`#a " not equal to " #b`) reads as written in the test
 * source. */
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
