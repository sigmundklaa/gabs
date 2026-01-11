
#ifndef GABS_HANDLE_HH__
#define GABS_HANDLE_HH__

#include <gabs/cc/include_guard.hh>
#include <gabs/core/handle.h>

namespace gabs::core
{

template <typename Handle> class simple_handle_trait
{
      public:
        using handle_type = Handle;

        const handle_type *to() const
        {
                return &handle_;
        }

        static simple_handle_trait *from(const handle_type *h)
        {
                return gabs_container_of(h, simple_handle_trait, handle_);
        }

      protected:
        handle_type handle_;
};

/**
 * @brief Wrapper around a handle class.
 *
 * Classes can inherit from this class to allow the class to be implictly
 * cast to the C-type handle, and get the C++ instance from a handle
 * using the `from_handle` method.
 *
 * `T` should be the inheriting class, and `HandleTrait` should be
 * a `simple_handle_trait`-like class that defines how to go to/from a
 * C-type handle.
 */
template <class T, class Handle> class handle_wrapper
{
      public:
        handle_wrapper() = default;

        template <typename... Args>
        handle_wrapper(Args... args) : handle_(args...)
        {
        }

        operator const typename Handle::handle_type *() const
        {
                return handle_.to();
        }

        static T *from_handle(const typename Handle::handle_type *h)
        {
                auto ptr = Handle::from(h);
                return gabs_container_of(ptr, T, handle_);
        }

      protected:
        Handle handle_;
};

}; // namespace gabs::core

#endif /* GABS_HANDLE_HH__ */
