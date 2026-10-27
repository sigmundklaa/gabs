
#ifndef GABS_TESTS_PBUF_ALLOC_H__
#define GABS_TESTS_PBUF_ALLOC_H__

#include <stddef.h>

#include <gabs/core/util.h>
#include <gabs/alloc.h>

GABS_BEGIN_DECL

/** @brief Allocator for the buffers of the pbuf implementation under test. */
const gabs_allocator_h *gabs_test_pbuf_alloc(void);

/** @brief Number of allocations from it that have not been released. */
size_t gabs_test_pbuf_outstanding(void);

GABS_END_DECL

#endif /* GABS_TESTS_PBUF_ALLOC_H__ */
