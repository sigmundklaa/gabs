
#include <gabs/alloc/zephyr/heap.h>

#include "../platform_alloc.hh"

K_HEAP_DEFINE(test_heap, 1024);
GABS_ALLOC_ZEPHYR_HEAP_DEFINE(test_allocator, &test_heap);

const gabs_allocator_h *gabs::tests::platform_allocator()
{
        return test_allocator;
}
