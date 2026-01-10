
#include <gabs/alloc/zephyr/heap.h>

static struct gabs_alloc_zephyr_heap *
get_container(const gabs_dyn_allocator *alloc)
{
        return gabs_container_of(alloc, struct gabs_alloc_zephyr_heap, dyn);
}

int GABS_DYN_ALLOC_IMPL(zephyr_heap, alloc)(const gabs_allocator_h *alloc,
                                            size_t size, void **mem)
{
        struct gabs_alloc_zephyr_heap *container;

        container = get_container(GABS_DYN_ALLOC_GET(alloc));
        *mem = k_heap_alloc(container->heap, size, K_NO_WAIT);

        if (*mem == NULL) {
                return -ENOMEM;
        }

        return 0;
}

int GABS_DYN_ALLOC_IMPL(zephyr_heap, dealloc)(const gabs_allocator_h *alloc,
                                              void *mem)
{
        struct gabs_alloc_zephyr_heap *container;

        container = get_container(GABS_DYN_ALLOC_GET(alloc));
        k_heap_free(container->heap, mem);

        return 0;
}
