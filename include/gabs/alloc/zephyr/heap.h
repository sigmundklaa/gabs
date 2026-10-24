
#ifndef GABS_ALLOC_ZEPHYR_HEAP_H__
#define GABS_ALLOC_ZEPHYR_HEAP_H__

#include <zephyr/kernel.h>

#include <gabs/core/util.h>
#include <gabs/alloc.h>

GABS_BEGIN_DECL

struct gabs_alloc_zephyr_heap {
        struct k_heap *heap;
        gabs_allocator allocator;
};

/** @brief API of allocators defined with GABS_ALLOC_ZEPHYR_HEAP_DEFINE. */
const struct gabs_allocator_api *gabs_zephyr_heap_alloc_api(void);

/* Only for GABS_ALLOC_ZEPHYR_HEAP_DEFINE. */
extern const struct gabs_allocator_api gabs_zephyr_heap_alloc_api__;

#define GABS_ALLOC_ZEPHYR_HEAP_DEFINE(name_, heap_)                            \
        static struct gabs_alloc_zephyr_heap name_##wrapper##__ = {            \
                .heap = heap_,                                                 \
                .allocator =                                                   \
                        GABS_ALLOCATOR_INIT(&gabs_zephyr_heap_alloc_api__),    \
        };                                                                     \
        static const gabs_allocator_h *name_ =                                 \
                GABS_ALLOCATOR_HANDLE(&name_##wrapper##__.allocator)

GABS_END_DECL

#endif /* GABS_ALLOC_ZEPHYR_HEAP_H__ */
