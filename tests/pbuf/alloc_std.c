
#include <stdatomic.h>

#include <gabs/alloc/std.h>

#include "pbuf_alloc.h"

static atomic_size_t outstanding;

static int counting_alloc(const gabs_allocator_h *allocator, size_t size,
                          void **mem)
{
        int status;

        GABS_UNUSED(allocator);

        status = gabs_alloc(gabs_std_allocator(), size, mem);
        if (status == 0) {
                atomic_fetch_add(&outstanding, 1);
        }

        return status;
}

static int counting_dealloc(const gabs_allocator_h *allocator, void *mem)
{
        GABS_UNUSED(allocator);

        atomic_fetch_sub(&outstanding, 1);

        return gabs_dealloc(gabs_std_allocator(), mem);
}

static const struct gabs_allocator_api counting_api = {
        .alloc = counting_alloc,
        .dealloc = counting_dealloc,
};

static const gabs_allocator counting_allocator =
        GABS_ALLOCATOR_INIT(&counting_api);

const gabs_allocator_h *gabs_test_pbuf_alloc(void)
{
        return GABS_ALLOCATOR_HANDLE(&counting_allocator);
}

size_t gabs_test_pbuf_outstanding(void)
{
        return atomic_load(&outstanding);
}
