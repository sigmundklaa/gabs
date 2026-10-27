
#ifndef GABS_TESTING_CATCH2_ASYNC_DEF_HH__
#define GABS_TESTING_CATCH2_ASYNC_DEF_HH__

#ifndef GABS_TESTING_INCLUDE_DEF
#error "gabs_testing_async_def.hh should not be included directly"
#endif /* GABS_TESTING_INCLUDE_DEF */

#include <future>
#include <utility>

namespace gabs::testing
{

template <typename F> auto async(F &&fn)
{
        return std::async(std::launch::async, std::forward<F>(fn));
}

} // namespace gabs::testing

#endif /* GABS_TESTING_CATCH2_ASYNC_DEF_HH__ */
