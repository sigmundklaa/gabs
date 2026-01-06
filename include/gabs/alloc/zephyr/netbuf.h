
#ifndef GABS_ALLOC_ZEPHYR_NETBUF_H__
#define GABS_ALLOC_ZEPHYR_NETBUF_H__

#include <zephyr/net_buf.h>

#include <gabs/alloc.h>
#include <gabs/alloc/dynamic.h>

typedef struct gabs_alloc_zephyr_netbuf {
        struct net_buf_pool *pool;
        gabs_dyn_allocator dyn;
} gabs_alloc_zephyr_netbuf;

void gabs_alloc_zephyr_netbuf_init(gabs_alloc_zephyr_netbuf *alloc,
                                   struct net_buf_pool *pool);

#endif /* GABS_ALLOC_ZEPHYR_NETBUF_H__ */
