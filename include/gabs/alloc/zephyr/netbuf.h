
#ifndef GABS_ALLOC_ZEPHYR_NETBUF_H__
#define GABS_ALLOC_ZEPHYR_NETBUF_H__

#include <zephyr/net_buf.h>

#include <gabs/alloc.h>
#include <gabs/alloc/dynamic.h>

GABS_BEGIN_DECL

struct gabs_alloc_zephyr_netbuf {
        struct net_buf_pool *pool;
        gabs_dyn_allocator dyn;
};

int GABS_DYN_ALLOC_IMPL(zephyr_netbuf, alloc)(const gabs_allocator_h *alloc,
                                              size_t size, void **mem);

int GABS_DYN_ALLOC_IMPL(zephyr_netbuf, dealloc)(const gabs_allocator_h *alloc,
                                                void *mem);

#define GABS_ALLOC_ZEPHYR_NETBUF_DEFINE(name_, pool_)                          \
        static struct gabs_alloc_zephyr_netbuf name_##wrapper##__ = {          \
                .pool = pool_,                                                 \
                .dyn = GABS_DYN_ALLOC_INIT(zephyr_netbuf),                     \
        };                                                                     \
        static const gabs_allocator_h *name_ =                                 \
                GABS_DYN_ALLOC_HANDLE(&name_##wrapper##__.dyn)

GABS_END_DECL

#endif /* GABS_ALLOC_ZEPHYR_NETBUF_H__ */
