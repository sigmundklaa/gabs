
#include <gabs/alloc/zephyr/netbuf.h>

#include "../pbuf_alloc.h"

#define POOL_COUNT 16

NET_BUF_POOL_VAR_DEFINE(test_pool, POOL_COUNT, 2048, 0, NULL);
GABS_ALLOC_ZEPHYR_NETBUF_DEFINE(test_allocator, &test_pool);

const gabs_allocator_h *gabs_test_pbuf_alloc(void)
{
        return test_allocator;
}

size_t gabs_test_pbuf_outstanding(void)
{
        return POOL_COUNT - net_buf_get_available(&test_pool);
}
