
#include "pbuf_alloc.h"

#include <gabs/alloc/std.h>

const gabs_allocator_h *gabs_test_pbuf_alloc(void)
{
        return gabs_std_allocator();
}
