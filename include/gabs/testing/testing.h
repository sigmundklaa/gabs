/**
 * The testing implementation must provide a `gabs_testing_def.h` header,
 * which defines the following macros:
 * - `GABS_SUITE(suite_)`
 * - `GABS_SUITE_F(suite_, fixture_)`
 * - `GABS_TEST(suite_, name_)`
 * - `GABS_TEST_F(suite_, name_)`
 * - `GABS_ASSERT_TRUE/FALSE(expr_)`
 * - `GABS_ASSERT_EQ/NE(a_, b_)`
 * - `GABS_ASSERT_NULL/NOT_NULL(ptr_)`
 * - `GABS_EXPECT_*`, mirroring every `GABS_ASSERT_*`
 * - `GABS_TEST_FAIL()`
 * - `GABS_TEST_SKIP()`
 *
 * Test sources are C++. A suite and its cases must be in the same
 * translation unit, and `GABS_TEST_F` requires its suite to be declared with
 * `GABS_SUITE_F`. The name `<suite>_fixture` is reserved for the
 * implementation.
 *
 * A fixture is a default-constructible class, constructed before each case
 * of the suite and destructed after it. `GABS_TEST_F` exposes it to the case
 * body as a pointer named `fixture`.
 *
 * `GABS_ASSERT_*` ends the case, and may do so by throwing. Fixture
 * destructors must use `GABS_EXPECT_*`, which records the failure and
 * continues.
 */

#ifndef GABS_TESTING_TESTING_H__
#define GABS_TESTING_TESTING_H__

#include <gabs/cc/include_guard.hh>

#define GABS_TESTING_INCLUDE_DEF
#include <gabs_testing_def.h>
#undef GABS_TESTING_INCLUDE_DEF

#endif /* GABS_TESTING_TESTING_H__ */
