
#ifndef GABS_ALLOC_H__
#define GABS_ALLOC_H__

#include <stddef.h>

#include <gabs/core/api.h>
#include <gabs/core/handle.h>
#include <gabs/core/util.h>

GABS_BEGIN_DECL

typedef gabs_handle gabs_allocator_h;

GABS_API int gabs_alloc(const gabs_allocator_h *allocator, size_t size,
                        void **mem);
GABS_API int gabs_dealloc(const gabs_allocator_h *allocator, void *mem);

GABS_END_DECL

#endif /* GABS_ALLOC_H__ */
