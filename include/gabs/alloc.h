
#ifndef GABS_ALLOC_H__
#define GABS_ALLOC_H__

#include <stddef.h>

#include <gabs/core/api.h>
#include <gabs/core/handle.h>
#include <gabs/core/util.h>

GABS_BEGIN_DECL

#define GABS_CONFIG_DYN_ALLOC 1

#define GABS_DYN_IMPL_NAME(x_)    gabs_impl_##x_##__
#define GABS_DYN_IMPL(x_)         static GABS_DYN_IMPL_NAME(x_)
#define GABS_STATIC_IMPL_NAME(x_) gabs_##x_
#define GABS_STATIC_IMPL(x_)      GABS_STATIC_IMPL_NAME(x_)

#if GABS_CONFIG_DYN_ALLOC
#define GABS_ALLOC_IMPL_NAME(x_) GABS_DYN_IMPL_NAME(x_)
#define GABS_ALLOC_IMPL(x_)      GABS_DYN_IMPL(x_)
#else
#define GABS_ALLOC_IMPL_NAME(x_) GABS_STATIC_IMPL_NAME(x_)
#define GABS_ALLOC_IMPL(x_)      GABS_STATIC_IMPL(x_)
#endif

typedef gabs_handle gabs_allocator_h;

GABS_API int gabs_alloc(const gabs_allocator_h *allocator, size_t size,
                        void **mem);
GABS_API int gabs_dealloc(const gabs_allocator_h *allocator, void *mem);

GABS_END_DECL

#endif /* GABS_ALLOC_H__ */
