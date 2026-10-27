
#include <gabs/alloc/std.h>

#include "platform_alloc.hh"

const gabs_allocator_h *gabs::tests::platform_allocator()
{
        return gabs_std_allocator();
}
