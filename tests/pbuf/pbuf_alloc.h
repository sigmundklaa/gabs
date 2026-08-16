
#ifndef GABS_TESTS_PBUF_ALLOC_H__
#define GABS_TESTS_PBUF_ALLOC_H__

#include <gabs/core/util.h>
#include <gabs/alloc.h>

GABS_BEGIN_DECL

/**
 * @brief Get the allocator to use for the pbuf test suite.
 *
 * The pbuf backends each expect a specific concrete allocator (e.g. the
 * Zephyr backend allocates `struct net_buf` instances out of a net_buf
 * pool), so which allocator this returns is provided per-backend rather
 * than by the platform-agnostic test suite itself.
 */
const gabs_allocator_h *gabs_test_pbuf_alloc(void);

GABS_END_DECL

#endif /* GABS_TESTS_PBUF_ALLOC_H__ */
