/**
 * The PBUF implemenation must provide a header named `gabs_pbuf_def.h`,
 * which defines the following types:
 * - `gabs_pbuf`
 * - `gabs_pbuf_ci`
 */

#ifndef GABS_PBUF_H__
#define GABS_PBUF_H__

#include <gabs/core/util.h>
#include <gabs/core/handle.h>
#include <gabs/alloc.h>

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#define GABS_PBUF_CI_TYPE_DEFINE(type_) typedef type_ gabs_pbuf_ci;
#define GABS_PBUF_TYPE_DEFINE(type_)    typedef type_ gabs_pbuf;

#define GABS_PBUF_IMPL_NAME(x_) GABS_STATIC_IMPL_NAME(x_)
#define GABS_PBUF_IMPL(x_)      GABS_STATIC_IMPL(x_)

/* Implementation files may want to both include the definitions directly, and
 * include this file. In that case, we don' want to undef the include macro.
 * This should only happen when using the dynamic implementation. */
#ifndef GABS_PBUF_INCLUDE_DEF
#define GABS_PBUF_INCLUDE_DEF
#include <gabs_pbuf_def.h>
#undef GABS_PBUF_INCLUDE_DEF
#else
#include <gabs_pbuf_def.h>
#endif

GABS_BEGIN_DECL

#define gabs_pbuf_ci_foreach(buf_, it_)                                        \
        for (it_ = gabs_pbuf_ci_init(buf_); !gabs_pbuf_ci_eoi(it_);            \
             it_ = gabs_pbuf_ci_next(it_))
/**
 * @brief Add @p next to chain of buffers, at a byte offset of @p offset.
 *
 * @note Assumes that the buffers are aligned at @p offset. Passing an @p offset
 * that is not zero bytes into a buffer is undefined behaviour.
 */
GABS_API void gabs_pbuf_chain_at(gabs_pbuf *buf, gabs_pbuf next, size_t offset);

/**
 * @brief Add @p back to the back of the chain poined to by @p buf
 */
GABS_API void gabs_pbuf_chain_back(gabs_pbuf *buf, gabs_pbuf back);

/**
 * @brief Add @p front to front of buffer chain.
 */
GABS_API void gabs_pbuf_chain_front(gabs_pbuf *buf, gabs_pbuf front);

/**
 * @brief Create a readonly view around @p buf, offset by @p offset bytes into
 * the buffer, with a size of @p size.
 *
 * @return gabs_pbuf*
 */
GABS_API gabs_pbuf gabs_pbuf_view(gabs_pbuf buf, size_t offset, size_t size,
                                  const gabs_allocator_h *alloc);

/**
 * @brief Create a deep copy of @p buf, offset by @p offset bytes with a size
 * of @p size.
 *
 * @return gabs_pbuf*
 */
GABS_API gabs_pbuf gabs_pbuf_clone(gabs_pbuf buf, size_t offset, size_t size,
                                   const gabs_allocator_h *alloc);

/**
 * @brief Put @p size from @p mem into the back of the buffer of @p buf.
 *
 * @note This only appends data to the specific buffer given in @p buf, not
 * at the end of the buffer chain.
 */
GABS_API void gabs_pbuf_put(gabs_pbuf *buf, const uint8_t *mem, size_t size);

GABS_API void gabs_pbuf_strip_head(gabs_pbuf *buf, size_t size);

GABS_API void gabs_pbuf_strip_tail(gabs_pbuf *buf, size_t size);

/**
 * @brief Copy @p max_size from an offset of @p offset bytes into @p buf into
 * @p mem.
 *
 * @return size_t Number of copied bytes
 */
GABS_API size_t gabs_pbuf_copy(gabs_pbuf buf, uint8_t *mem, size_t offset,
                               size_t max_size);

/** @brief Return the total size of the buffer chain */
GABS_API size_t gabs_pbuf_size(gabs_pbuf buf);

GABS_API size_t gabs_pbuf_tailroom(gabs_pbuf buf);

GABS_API size_t gabs_pbuf_headroom(gabs_pbuf buf);

GABS_API gabs_pbuf gabs_pbuf_new(const gabs_allocator_h *alloc, size_t size);
GABS_API gabs_pbuf gabs_pbuf_new_ro(const gabs_allocator_h *alloc,
                                    uint8_t *data, size_t size);

GABS_API bool gabs_pbuf_okay(gabs_pbuf buf);
GABS_API void gabs_pbuf_incref(gabs_pbuf buf);
GABS_API void gabs_pbuf_decref(gabs_pbuf buf);

GABS_API gabs_pbuf_ci gabs_pbuf_ci_init(gabs_pbuf *buf);
GABS_API gabs_pbuf_ci gabs_pbuf_ci_next(gabs_pbuf_ci it);
GABS_API bool gabs_pbuf_ci_eoi(gabs_pbuf_ci it);
GABS_API uint8_t *gabs_pbuf_ci_data(gabs_pbuf_ci it);
GABS_API uint8_t *gabs_pbuf_ci_reserve_head(gabs_pbuf_ci it, size_t bytes);
GABS_API uint8_t *gabs_pbuf_ci_release_head(gabs_pbuf_ci it, size_t bytes);
GABS_API uint8_t *gabs_pbuf_ci_reserve_tail(gabs_pbuf_ci it, size_t bytes);
GABS_API uint8_t *gabs_pbuf_ci_release_tail(gabs_pbuf_ci it, size_t bytes);
GABS_API size_t gabs_pbuf_ci_headroom(gabs_pbuf_ci it);
GABS_API size_t gabs_pbuf_ci_tailroom(gabs_pbuf_ci it);
GABS_API gabs_pbuf_ci gabs_pbuf_ci_insert(gabs_pbuf_ci it, gabs_pbuf buf);
GABS_API gabs_pbuf_ci gabs_pbuf_ci_remove(gabs_pbuf_ci it);
GABS_API void gabs_pbuf_ci_detach(gabs_pbuf_ci it);
GABS_API size_t gabs_pbuf_ci_cap(gabs_pbuf_ci it);
GABS_API size_t gabs_pbuf_ci_size(gabs_pbuf_ci it);

GABS_END_DECL

#endif /* GABS_PBUF_H__ */
