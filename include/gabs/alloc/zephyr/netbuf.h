
#ifndef GABS_ALLOC_ZEPHYR_NETBUF_H__
#define GABS_ALLOC_ZEPHYR_NETBUF_H__

#include <zephyr/net_buf.h>

#include <gabs/alloc.h>

GABS_BEGIN_DECL

struct gabs_alloc_zephyr_netbuf {
        struct net_buf_pool *pool;
        gabs_allocator allocator;
};

/** @brief API of allocators defined with GABS_ALLOC_ZEPHYR_NETBUF_DEFINE. */
const struct gabs_allocator_api *gabs_zephyr_netbuf_alloc_api(void);

/* Only for GABS_ALLOC_ZEPHYR_NETBUF_DEFINE. */
extern const struct gabs_allocator_api gabs_zephyr_netbuf_alloc_api__;

#define GABS_ALLOC_ZEPHYR_NETBUF_DEFINE(name_, pool_)                          \
        static struct gabs_alloc_zephyr_netbuf name_##wrapper##__ = {          \
                .pool = pool_,                                                 \
                .allocator =                                                   \
                        GABS_ALLOCATOR_INIT(&gabs_zephyr_netbuf_alloc_api__),  \
        };                                                                     \
        static const gabs_allocator_h *name_ =                                 \
                GABS_ALLOCATOR_HANDLE(&name_##wrapper##__.allocator)

GABS_END_DECL

#endif /* GABS_ALLOC_ZEPHYR_NETBUF_H__ */
