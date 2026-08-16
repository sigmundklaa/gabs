
#include "../pbuf_alloc.h"

#include <gabs/alloc/zephyr/netbuf.h>

NET_BUF_POOL_DEFINE(gabs_test_pbuf_pool__, 4, 128, 0, NULL);
GABS_ALLOC_ZEPHYR_NETBUF_DEFINE(gabs_test_pbuf_allocator__, &gabs_test_pbuf_pool__);

const gabs_allocator_h *gabs_test_pbuf_alloc(void)
{
        return gabs_test_pbuf_allocator__;
}
