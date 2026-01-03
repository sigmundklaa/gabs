
#ifndef GABS_CC_DEFER_HH__
#define GABS_CC_DEFER_HH__

#include <gabs/cc/include_guard.hh>

#include <utility>

namespace gabs::util
{

template <class Func> class defer
{
      public:
        defer(Func &&fn) : fn(std::move(fn))
        {
        }

        ~defer()
        {
                fn();
        }

      private:
        Func fn;
};

}; // namespace gabs::util

#endif /* GABS_UTIL_HH__ */
