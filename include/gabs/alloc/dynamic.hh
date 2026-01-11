
#ifndef GABS_DYNAMIC_HH__
#define GABS_DYNAMIC_HH__

#include <gabs/cc/include_guard.hh>
#include <gabs/alloc/dynamic.h>

#if !defined(GABS_CONFIG_DYN_ALLOC) || !GABS_CONFIG_DYN_ALLOC
#error "Dynamic allocator support required"
#endif

namespace gabs::memory
{

class dyn_alloc
{
      public:
        using handle_type = ::gabs_allocator_h;

        dyn_alloc(::gabs_dyn_allocator_alloc alloc,
                  ::gabs_dyn_allocator_dealloc dealloc)
                : alloc_(GABS_DYN_ALLOC_INIT(alloc, dealloc))
        {
        }

        const handle_type *to() const
        {
                return GABS_DYN_ALLOC_HANDLE(&alloc_);
        }

        static dyn_alloc *from(const handle_type *h)
        {
                auto ptr = GABS_DYN_ALLOC_GET(h);
                return gabs_container_of(ptr, dyn_alloc, alloc_);
        }

      protected:
        ::gabs_dyn_allocator alloc_;
};

}; // namespace gabs::memory

#endif /* GABS_DYNAMIC_HH__ */
