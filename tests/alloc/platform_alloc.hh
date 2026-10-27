
#ifndef GABS_TESTS_PLATFORM_ALLOC_HH__
#define GABS_TESTS_PLATFORM_ALLOC_HH__

#include <gabs/alloc.h>

namespace gabs::tests
{

/** @brief General purpose allocator of the platform. */
const gabs_allocator_h *platform_allocator();

} // namespace gabs::tests

#endif /* GABS_TESTS_PLATFORM_ALLOC_HH__ */
