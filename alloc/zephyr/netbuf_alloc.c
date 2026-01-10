
#include <gabs/alloc/zephyr/netbuf.h>

static struct gabs_alloc_zephyr_netbuf *
get_container(const gabs_dyn_allocator *alloc)
{
        return gabs_container_of(alloc, struct gabs_alloc_zephyr_netbuf, dyn);
}

int GABS_DYN_ALLOC_IMPL(zephyr_netbuf, alloc)(const gabs_allocator_h *alloc,
                                              size_t size, void **mem)
{
        struct net_buf *buf;
        struct gabs_alloc_zephyr_netbuf *container;

        container = get_container(GABS_DYN_ALLOC_GET(alloc));
        buf = net_buf_alloc_len(container->pool, size, K_NO_WAIT);

        if (buf == NULL) {
                return -ENOMEM;
        }

        *mem = buf;
        return 0;
}

int GABS_DYN_ALLOC_IMPL(zephyr_netbuf, dealloc)(const gabs_allocator_h *alloc,
                                                void *mem)
{
        __ASSERT(0, "Buffer should never be deallocated through gabs_dealloc");
        return -ENOTSUP;
}
