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

/**
 * @defgroup gabs_pbuf Packet buffer API
 *
 * @{
 */

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

/**
 * @brief Iterate over each contiguous section of the buffer @p buf_.
 *
 * The iterator must be pre-allocated and its variable name should be given in
 * @p it_.
 */
#define gabs_pbuf_ci_foreach(buf_, it_)                                        \
        for (it_ = gabs_pbuf_ci_init(buf_); !gabs_pbuf_ci_eoi(it_);            \
             it_ = gabs_pbuf_ci_next(it_))

/**
 * @brief Allocate and construct new buffer, with size @p size.
 *
 * @return gabs_pbuf
 * @retval !gabs_pbuf_okay(...) Allocation failed
 */
GABS_API gabs_pbuf gabs_pbuf_new(const gabs_allocator_h *alloc, size_t size);

/**
 * @brief Allocate and construct buffer as a readonly view over the contents
 * specified by @p data.
 *
 * @param alloc
 * @param data Underlying data to create a view over
 * @param size Size of underlying data
 * @return gabs_pbuf See @ref gabs_pbuf_new for details
 */
GABS_API gabs_pbuf gabs_pbuf_new_ro(const gabs_allocator_h *alloc,
                                    uint8_t *data, size_t size);

/**
 * @brief Check whether buffer is valid or not
 *
 * If false is returned, the buffer should not be used.
 * @return bool
 * @retval true Buffer valid
 * @retval false Buffer invalid
 */
GABS_API bool gabs_pbuf_okay(gabs_pbuf buf);

/** @brief Increase reference counter of buffer */
GABS_API void gabs_pbuf_incref(gabs_pbuf buf);

/** brief Decrease reference counter of buffer, potentially deallocating. */
GABS_API void gabs_pbuf_decref(gabs_pbuf buf);

/** @brief Return the total size of the buffer chain */
GABS_API size_t gabs_pbuf_size(gabs_pbuf buf);

/** @brief Return number of bytes available at the back of the buffer */
GABS_API size_t gabs_pbuf_tailroom(gabs_pbuf buf);

/** @brief Return number of bytes available at the front of the buffer */
GABS_API size_t gabs_pbuf_headroom(gabs_pbuf buf);

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
 * @brief Put @p size from @p mem into the back of the buffer of @p buf.
 *
 * @note This only appends data to the specific buffer given in @p buf, not
 * at the end of the buffer chain.
 */
GABS_API void gabs_pbuf_put(gabs_pbuf *buf, const uint8_t *mem, size_t size);

/**
 * @brief Strip @p size bytes off the head of the buffer, potentially
 * deallocating now empty sections.
 */
GABS_API void gabs_pbuf_strip_head(gabs_pbuf *buf, size_t size);

/**
 * @brief Strip @p size bytes off the tail of the buffer, potentially
 * deallocating now empty sections.
 */
GABS_API void gabs_pbuf_strip_tail(gabs_pbuf *buf, size_t size);

/**
 * @brief Copy @p max_size from an offset of @p offset bytes into @p buf into
 * @p mem.
 *
 * @return size_t Number of copied bytes
 */
GABS_API size_t gabs_pbuf_copy(gabs_pbuf buf, uint8_t *mem, size_t offset,
                               size_t max_size);

/**
 * @defgroup gabs_pbuf_ci Contiguous iterators
 *
 * The basic building block of the packet buffer API are the contiguous
 * iterators; these are the core of what an implementation needs to define,
 * and as the name implies, they are used to iterate over the parts
 * of the buffer which are contiguous in memory
 *
 * @{
 * */

/**
 * @brief Initialize and return an iterator for @p buf
 *
 * The validity of the iterator can be checked with @ref gabs_pbuf_ci_eoi.
 *
 * @return gabs_pbuf_ci
 */
GABS_API gabs_pbuf_ci gabs_pbuf_ci_init(gabs_pbuf *buf);

/**
 * @brief Advance the iterator
 *
 * The returned iterator will either point to the next segment in the buffer,
 * or it will mark the end of the iteration, which can be tested with
 * @ref gabs_pbuf_ci_eoi.
 *
 * @return gabs_pbuf_ci
 */
GABS_API gabs_pbuf_ci gabs_pbuf_ci_next(gabs_pbuf_ci it);

/**
 * @brief Check for end of iteration for iterator @p it
 *
 * @return bool
 * @retval false @p it is not the end of the iteration
 * @retval true @p it is the end of the iteration
 */
GABS_API bool gabs_pbuf_ci_eoi(gabs_pbuf_ci it);

/**
 * @brief Get pointer to contiguous underlying data for the current
 * segment pointed to by the iterator.
 *
 * @return uint8_t*
 */
GABS_API uint8_t *gabs_pbuf_ci_data(gabs_pbuf_ci it);

/**
 * @brief Reserve @p bytes at the beginning of the segment, returning the
 * memory of the new head of the buffer.
 */
GABS_API uint8_t *gabs_pbuf_ci_reserve_head(gabs_pbuf_ci it, size_t bytes);

/**
 * @brief Release @p bytes at the beginning of the segment, returning the
 * memory of the old head of the buffer.
 */
GABS_API uint8_t *gabs_pbuf_ci_release_head(gabs_pbuf_ci it, size_t bytes);

/**
 * @brief Reserve @p bytes at the end of the segment, returning the memory
 * of the new tail of the buffer.
 */
GABS_API uint8_t *gabs_pbuf_ci_reserve_tail(gabs_pbuf_ci it, size_t bytes);

/** @brief Release @p bytes at the end of the segemnt, returning the memory
 * of the old tail of the buffer. */
GABS_API uint8_t *gabs_pbuf_ci_release_tail(gabs_pbuf_ci it, size_t bytes);

/** @brief Get bytes available at front of segment. */
GABS_API size_t gabs_pbuf_ci_headroom(gabs_pbuf_ci it);

/** @brief Get bytes available at back of segment. */
GABS_API size_t gabs_pbuf_ci_tailroom(gabs_pbuf_ci it);

/**
 * @brief Insert @p buf into the underlying buffer pointed to by @p it,
 * placing it as the element immediately following @p it.
 *
 * The returned iterator is the next element in the sequence, which now is
 * an iterator corresponding to the head of @p buf.
 */
GABS_API gabs_pbuf_ci gabs_pbuf_ci_insert(gabs_pbuf_ci it, gabs_pbuf buf);

/**
 * @brief Remove the contiguous segment pointed to by @p it from the
 * underlying buffer.
 *
 * The returned iterator is the next contiguous segment immediately following
 * that of @p it.
 */
GABS_API gabs_pbuf_ci gabs_pbuf_ci_remove(gabs_pbuf_ci it);

/**
 * @brief Detach the current contigous segment and all following segments
 * from the underlying buffer.
 */
GABS_API void gabs_pbuf_ci_detach(gabs_pbuf_ci it);

/** @brief Get total (allocated) capacity of contigous segment. */
GABS_API size_t gabs_pbuf_ci_cap(gabs_pbuf_ci it);

/** @brief Get used size of contiguous segment. */
GABS_API size_t gabs_pbuf_ci_size(gabs_pbuf_ci it);

/** @} */

/** @} */

GABS_END_DECL

#endif /* GABS_PBUF_H__ */
