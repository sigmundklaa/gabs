
#ifndef GABS_TESTS_PBUF_ALLOC_HH__
#define GABS_TESTS_PBUF_ALLOC_HH__

#include <stddef.h>

#include <gabs/alloc.h>

namespace gabs::tests
{

/**
 * @brief Allocator for the buffers of the pbuf implementation under test.
 *
 * Each implementation's tests provide a subclass, and @ref instance, which
 * returns the one object of it.
 */
class pbuf_allocator
{
      public:
        static pbuf_allocator &instance();

        virtual const gabs_allocator_h *handle() = 0;

        /** @brief Number of allocations that have not been released. */
        virtual size_t outstanding() = 0;

      protected:
        ~pbuf_allocator() = default;
};

} // namespace gabs::tests

#endif /* GABS_TESTS_PBUF_ALLOC_HH__ */
