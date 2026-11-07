
#include <atomic>

#include <gabs/alloc/std.h>

#include "pbuf_alloc.hh"

namespace
{

class std_allocator : public gabs::tests::pbuf_allocator
{
      public:
        const gabs_allocator_h *handle() override
        {
                return GABS_ALLOCATOR_HANDLE(&allocator_);
        }

        size_t outstanding() override
        {
                return outstanding_;
        }

      private:
        static int alloc(const gabs_allocator_h *, size_t size, void **mem)
        {
                int status = gabs_alloc(gabs_std_allocator(), size, mem);

                if (status == 0) {
                        outstanding_++;
                }

                return status;
        }

        static int dealloc(const gabs_allocator_h *, void *mem)
        {
                outstanding_--;

                return gabs_dealloc(gabs_std_allocator(), mem);
        }

        static constexpr gabs_allocator_api api_ = {
                .alloc = alloc,
                .dealloc = dealloc,
        };

        static inline std::atomic_size_t outstanding_;
        gabs_allocator allocator_ = GABS_ALLOCATOR_INIT(&api_);
};

} // namespace

gabs::tests::pbuf_allocator &gabs::tests::pbuf_allocator::instance()
{
        static std_allocator allocator;

        return allocator;
}
