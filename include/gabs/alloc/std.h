
#ifndef GABS_ALLOC_STD_H__
#define GABS_ALLOC_STD_H__

#include <gabs/alloc/dynamic.h>

GABS_BEGIN_DECL

int GABS_DYN_ALLOC_IMPL(std, alloc)(const gabs_allocator_h *alloc, size_t size,
                                    void **mem);

int GABS_DYN_ALLOC_IMPL(std, dealloc)(const gabs_allocator_h *alloc, void *mem);

#if GABS_CONFIG_DYN_ALLOC
const gabs_allocator_h *gabs_std_allocator(void);
#else
static inline const gabs_allocator_h *gabs_std_allocator(void)
{
        return NULL;
}
#endif

GABS_END_DECL

#endif /* GABS_ALLOC_STD_H__ */
