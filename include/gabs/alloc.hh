
#ifndef GABS_ALLOC_HH__
#define GABS_ALLOC_HH__

#include <gabs/cc/include_guard.hh>
#include <gabs/alloc.h>
#include <gabs/cc/handle.hh>

namespace gabs::memory
{

class allocator_handle
{
      public:
        using handle_type = ::gabs_allocator_h;

        allocator_handle(const ::gabs_allocator_api *api)
                : alloc_(GABS_ALLOCATOR_INIT(api))
        {
        }

        const handle_type *to() const
        {
                return GABS_ALLOCATOR_HANDLE(&alloc_);
        }

        static allocator_handle *from(const handle_type *h)
        {
                return &core::container_of(GABS_ALLOCATOR_GET(h),
                                           &allocator_handle::alloc_);
        }

      protected:
        ::gabs_allocator alloc_;
};

}; // namespace gabs::memory

#endif /* GABS_ALLOC_HH__ */
