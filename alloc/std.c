
#include <stdlib.h>
#include <errno.h>

#include <gabs/alloc.h>
#include <gabs/alloc/std.h>

static int std_alloc(const gabs_allocator_h *allocator, size_t size,
                     void **mem)
{
        GABS_UNUSED(allocator);

        *mem = malloc(size);
        if (*mem == NULL) {
                return -ENOMEM;
        }

        return 0;
}

static int std_dealloc(const gabs_allocator_h *allocator, void *mem)
{
        GABS_UNUSED(allocator);

        free(mem);
        return 0;
}

static const struct gabs_allocator_api std_api = {
        .alloc = std_alloc,
        .dealloc = std_dealloc,
};

const struct gabs_allocator_api *gabs_std_alloc_api(void)
{
        return &std_api;
}

const gabs_allocator_h *gabs_std_allocator(void)
{
        static const gabs_allocator allocator =
                GABS_ALLOCATOR_INIT(&std_api);

        return GABS_ALLOCATOR_HANDLE(&allocator);
}
