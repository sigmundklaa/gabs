
#include <gabs/alloc/zephyr/netbuf.h>

#include "../pbuf_alloc.hh"

namespace
{

constexpr size_t pool_count = 16;

NET_BUF_POOL_VAR_DEFINE(test_pool, pool_count, 2048, 0, NULL);
GABS_ALLOC_ZEPHYR_NETBUF_DEFINE(test_allocator, &test_pool);

class netbuf_allocator : public gabs::tests::pbuf_allocator
{
      public:
        const gabs_allocator_h *handle() override
        {
                return test_allocator;
        }

        size_t outstanding() override
        {
                return pool_count - net_buf_get_available(&test_pool);
        }
};

} // namespace

gabs::tests::pbuf_allocator &gabs::tests::pbuf_allocator::instance()
{
        static netbuf_allocator allocator;

        return allocator;
}
