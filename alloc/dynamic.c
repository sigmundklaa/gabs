
#include <gabs/alloc.h>
#include <gabs/alloc/dynamic.h>

void gabs_dyn_allocator_init(gabs_dyn_allocator *allocator,
                             gabs_dyn_allocator_alloc alloc,
                             gabs_dyn_allocator_dealloc dealloc)
{
        allocator->alloc = alloc;
        allocator->dealloc = dealloc;

        gabs_handle_init(&allocator->handle);
}

int gabs_alloc(const gabs_allocator_h *allocator, size_t size, void** mem)
{
        return gabs_dyn_allocator_get(allocator)->alloc(allocator, size, mem);
}

int gabs_dealloc(const gabs_allocator_h *allocator, void *mem)
{
        return gabs_dyn_allocator_get(allocator)->dealloc(allocator, mem);
}
