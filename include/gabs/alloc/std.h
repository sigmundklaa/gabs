
#ifndef GABS_ALLOC_STD_H__
#define GABS_ALLOC_STD_H__

#include <gabs/alloc.h>

GABS_BEGIN_DECL

/** @brief API of the allocator returned by gabs_std_allocator(). */
const struct gabs_allocator_api *gabs_std_alloc_api(void);

const gabs_allocator_h *gabs_std_allocator(void);

GABS_END_DECL

#endif /* GABS_ALLOC_STD_H__ */
