
#include <gabs/alloc/zephyr/heap.h>

static struct gabs_alloc_zephyr_heap *
get_container(const gabs_allocator *alloc)
{
        return gabs_container_of(alloc, struct gabs_alloc_zephyr_heap,
                                 allocator);
}

static int heap_alloc(const gabs_allocator_h *alloc, size_t size, void **mem)
{
        struct gabs_alloc_zephyr_heap *container;

        container = get_container(GABS_ALLOCATOR_GET(alloc));
        *mem = k_heap_alloc(container->heap, size, K_NO_WAIT);

        if (*mem == NULL) {
                return -ENOMEM;
        }

        return 0;
}

static int heap_dealloc(const gabs_allocator_h *alloc, void *mem)
{
        struct gabs_alloc_zephyr_heap *container;

        container = get_container(GABS_ALLOCATOR_GET(alloc));
        k_heap_free(container->heap, mem);

        return 0;
}

const struct gabs_allocator_api gabs_zephyr_heap_alloc_api__ = {
        .alloc = heap_alloc,
        .dealloc = heap_dealloc,
};

const struct gabs_allocator_api *gabs_zephyr_heap_alloc_api(void)
{
        return &gabs_zephyr_heap_alloc_api__;
}
