
#ifndef GABS_ALLOC_DYNAMIC_H__
#define GABS_ALLOC_DYNAMIC_H__

#include <gabs/alloc.h>

GABS_BEGIN_DECL

#if GABS_CONFIG_DYN_ALLOC
typedef int (*gabs_dyn_allocator_alloc)(const gabs_allocator_h *, size_t,
                                        void **);
typedef int (*gabs_dyn_allocator_dealloc)(const gabs_allocator_h *, void *);

struct gabs_dyn_allocator {
        gabs_dyn_allocator_alloc alloc;
        gabs_dyn_allocator_dealloc dealloc;
        gabs_allocator_h handle;
};

void gabs_dyn_allocator_init__(struct gabs_dyn_allocator *allocator,
                               gabs_dyn_allocator_alloc alloc,
                               gabs_dyn_allocator_dealloc dealloc);

#define gabs_dyn_allocator_init(allocator_, alloc_, dealloc_)                  \
        gabs_dyn_allocator_init__(allocator_, alloc_, dealloc_)
#else
struct gabs_dyn_allocator {
        gabs_allocator_h handle;
};

#define gabs_dyn_allocator_init(allocator_, alloc_, dealloc_)                  \
        gabs_handle_init(&(allocator_)->handle)
#endif

typedef struct gabs_dyn_allocator gabs_dyn_allocator;

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

GABS_END_DECL

#endif /* GABS_ALLOC_DYNAMIC_H__ */
