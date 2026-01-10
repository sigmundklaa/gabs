
#include <gabs/alloc.h>
#include <gabs/alloc/dynamic.h>

int gabs_alloc(const gabs_allocator_h *allocator, size_t size, void **mem)
{
        return GABS_DYN_ALLOC_GET(allocator)->alloc(allocator, size, mem);
}

int gabs_dealloc(const gabs_allocator_h *allocator, void *mem)
{
        return GABS_DYN_ALLOC_GET(allocator)->dealloc(allocator, mem);
}
