
#include <gabs/alloc.h>

int gabs_alloc(const gabs_allocator_h *allocator, size_t size, void **mem)
{
        return GABS_ALLOCATOR_GET(allocator)->api->alloc(allocator, size, mem);
}

int gabs_dealloc(const gabs_allocator_h *allocator, void *mem)
{
        return GABS_ALLOCATOR_GET(allocator)->api->dealloc(allocator, mem);
}

bool gabs_allocator_is(const gabs_allocator_h *allocator,
                       const struct gabs_allocator_api *api)
{
        return GABS_ALLOCATOR_GET(allocator)->api == api;
}
