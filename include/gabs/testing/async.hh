/**
 * The testing implementation must provide a `gabs_testing_async_def.hh`
 * header, which defines `gabs::testing::async(fn)`.
 *
 * `async` calls `fn` without arguments in another thread, and returns a task
 * whose `get()` waits for `fn` to return and returns its result. `get()` may
 * be called once. Destroying the task also waits for `fn`.
 *
 * Tasks may run one at a time, in the order they were started, so a task
 * must not wait for one started after it. `fn` must not use `GABS_ASSERT_*`
 * or `GABS_EXPECT_*`; return what the case should check instead.
 */

#ifndef GABS_TESTING_ASYNC_HH__
#define GABS_TESTING_ASYNC_HH__

#include <gabs/cc/include_guard.hh>

#define GABS_TESTING_INCLUDE_DEF
#include <gabs_testing_async_def.hh>
#undef GABS_TESTING_INCLUDE_DEF

#endif /* GABS_TESTING_ASYNC_HH__ */
