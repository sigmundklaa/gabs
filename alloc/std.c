
#include <stdlib.h>
#include <errno.h>

#include <gabs/alloc/dynamic.h>
#include <gabs/alloc/std.h>

int GABS_ALLOC_IMPL(alloc)(const gabs_allocator_h *allocator, size_t size,
                           void **mem)
{
        GABS_UNUSED(allocator);

        *mem = malloc(size);
        if (*mem == NULL) {
                return -ENOMEM;
        }

        return 0;
}

int GABS_ALLOC_IMPL(dealloc)(const gabs_allocator_h *allocator, void *mem)
{
        GABS_UNUSED(allocator);

        free(mem);
        return 0;
}

#ifdef GABS_CONFIG_DYN_ALLOC
const gabs_allocator_h *gabs_std_allocator(void)
{
        static const gabs_dyn_allocator allocator = {
                .alloc = GABS_ALLOC_IMPL_NAME(alloc),
                .dealloc = GABS_ALLOC_IMPL_NAME(dealloc),
        };

        return gabs_dyn_allocator_handle(&allocator);
}
#endif
