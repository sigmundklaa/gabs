
#ifndef GABS_ALLOC_ZEPHYR_HEAP_H__
#define GABS_ALLOC_ZEPHYR_HEAP_H__

#include <zephyr/kernel.h>

#include <gabs/core/util.h>
#include <gabs/alloc/dynamic.h>

GABS_BEGIN_DECL

struct gabs_alloc_zephyr_heap {
        struct k_heap *heap;
        gabs_dyn_allocator dyn;
};

int GABS_DYN_ALLOC_IMPL(zephyr_heap, alloc)(const gabs_allocator_h *alloc,
                                            size_t size, void **mem);

int GABS_DYN_ALLOC_IMPL(zephyr_heap, dealloc)(const gabs_allocator_h *alloc,
                                              void *mem);

#define GABS_ALLOC_ZEPHYR_HEAP_DEFINE(name_, heap_)                            \
        static struct gabs_alloc_zephyr_heap name_##wrapper##__ = {            \
                .heap = heap_,                                                 \
                .dyn = GABS_DYN_ALLOC_INIT(zephyr_heap),                       \
        };                                                                     \
        static const gabs_allocator_h *name_ =                                 \
                GABS_DYN_ALLOC_HANDLE(&name_##wrapper##__.dyn)

GABS_END_DECL

#endif /* GABS_ALLOC_ZEPHYR_HEAP_H__ */
