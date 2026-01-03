
#ifndef GABS_ALLOC_DYNAMIC_H__
#define GABS_ALLOC_DYNAMIC_H__

#include <gabs/alloc.h>

GABS_BEGIN_DECL

typedef int (*gabs_dyn_allocator_alloc)(const gabs_allocator_h *, size_t,
                                        void **);
typedef int (*gabs_dyn_allocator_dealloc)(const gabs_allocator_h *, void *);

typedef struct gabs_dyn_allocator {
        gabs_dyn_allocator_alloc alloc;
        gabs_dyn_allocator_dealloc dealloc;
        gabs_allocator_h handle;
} gabs_dyn_allocator;

static inline const gabs_allocator_h *
gabs_dyn_allocator_handle(const struct gabs_dyn_allocator *allocator)
{
        return &allocator->handle;
}

static inline const struct gabs_dyn_allocator *
gabs_dyn_allocator_get(const gabs_allocator_h *handle)
{
        return gabs_container_of(handle, struct gabs_dyn_allocator, handle);
}

void gabs_dyn_allocator_init(gabs_dyn_allocator *allocator,
                             gabs_dyn_allocator_alloc alloc,
                             gabs_dyn_allocator_dealloc dealloc);

GABS_END_DECL

#endif /* GABS_ALLOC_DYNAMIC_H__ */
