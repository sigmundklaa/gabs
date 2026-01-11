
#ifndef GABS_ALLOC_STD_HH__
#define GABS_ALLOC_STD_HH__

#include <gabs/cc/include_guard.hh>

#include <memory>
#include <cstring>

#include <gabs/alloc.h>
#include <gabs/cc/handle.hh>
#include <gabs/alloc/dynamic.hh>

namespace gabs::memory
{

namespace detail
{

template <class Allocator>
int do_alloc(const ::gabs_allocator_h *handle, size_t size, void **mem);

template <class Allocator>
int do_dealloc(const ::gabs_allocator_h *handle, void *mem);

}; // namespace detail

template <template <class> class BaseAllocator = std::allocator>
class allocator
        : public core::handle_wrapper<allocator<BaseAllocator>, dyn_alloc>
{
      public:
        using value_type = std::byte;

        allocator(const std::shared_ptr<BaseAllocator<value_type>> &underlying)
                : underlying_(underlying),
                  core::handle_wrapper<allocator, dyn_alloc>(
                          &detail::do_alloc<allocator>,
                          &detail::do_dealloc<allocator>)
        {
        }

        allocator() : allocator(std::make_shared<BaseAllocator<value_type>>())
        {
        }

        const BaseAllocator<value_type> &underlying() const
        {
                return *underlying_;
        }

      protected:
        std::shared_ptr<BaseAllocator<value_type>> underlying_;
};

namespace detail
{

template <class Allocator>
int do_alloc(const ::gabs_allocator_h *handle, std::size_t size, void **mem)
{
        using T = typename Allocator::value_type;

        auto alloc = Allocator::from_handle(handle);
        auto inst = alloc->underlying();

        T *ptr = inst.allocate(sizeof(size) + size);
        std::memcpy(ptr, &size, sizeof(size));

        *mem = ((std::byte *)ptr) + sizeof(size);

        return 0;
}

template <class Allocator>
int do_dealloc(const ::gabs_allocator_h *handle, void *mem)
{
        using T = typename Allocator::value_type;

        auto alloc = Allocator::from_handle(handle);
        auto inst = alloc->underlying();

        std::size_t size;
        std::byte *base = reinterpret_cast<std::byte *>(mem) - sizeof(size);

        std::memcpy(&size, base, sizeof(size));
        inst.deallocate(reinterpret_cast<T *>(base), sizeof(size) + size);

        return 0;
}

}; // namespace detail

}; // namespace gabs::memory

#endif /* GABS_ALLOC_STD_HH__ */
