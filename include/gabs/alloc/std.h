
#ifndef GABS_ALLOC_STD_H__
#define GABS_ALLOC_STD_H__

#include <gabs/alloc.h>

GABS_BEGIN_DECL

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
