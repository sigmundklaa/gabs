
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

#define GABS_DYN_ALLOC_IMPL(impl_, method_)                                    \
        gabs_impl_dyn_alloc_##method_##_##impl_##__

#define GABS_DYN_ALLOC_INIT(impl_)                                             \
        (struct gabs_dyn_allocator)                                            \
        {                                                                      \
                .alloc = GABS_DYN_ALLOC_IMPL(impl_, alloc),                    \
                .dealloc = GABS_DYN_ALLOC_IMPL(impl_, dealloc),                \
        }

#else
struct gabs_dyn_allocator {
        gabs_allocator_h handle;
};

#define GABS_DYN_ALLOC_IMPL(impl_, method_) gabs_##method_

#define GABS_DYN_ALLOC_INIT(impl_)                                             \
        (struct gabs_dyn_allocator)                                            \
        {                                                                      \
        }
#endif

typedef struct gabs_dyn_allocator gabs_dyn_allocator;

#define GABS_DYN_ALLOC_HANDLE(alloc_) (&(alloc_)->handle)

static inline const struct gabs_dyn_allocator *
gabs_dyn_allocator_get(const gabs_allocator_h *handle)
{
        return gabs_container_of(handle, struct gabs_dyn_allocator, handle);
}

GABS_END_DECL

#endif /* GABS_ALLOC_DYNAMIC_H__ */
