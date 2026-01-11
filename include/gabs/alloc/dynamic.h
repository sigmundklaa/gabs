
#ifndef GABS_ALLOC_DYNAMIC_H__
#define GABS_ALLOC_DYNAMIC_H__

#include <gabs/alloc.h>
#include <gabs/core/util.h>

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

#define GABS_DYN_ALLOC_INIT_2__(alloc_, dealloc_)                              \
        (struct gabs_dyn_allocator)                                            \
        {                                                                      \
                .alloc = alloc_, .dealloc = dealloc_,                          \
        }

#else
struct gabs_dyn_allocator {
        gabs_allocator_h handle;
};

#define GABS_DYN_ALLOC_IMPL(impl_, method_) gabs_##method_

#define GABS_DYN_ALLOC_INIT_2__(...)                                           \
        (struct gabs_dyn_allocator)                                            \
        {                                                                      \
        }
#endif

typedef struct gabs_dyn_allocator gabs_dyn_allocator;

#define GABS_DYN_ALLOC_INIT_1__(impl_)                                         \
        GABS_DYN_ALLOC_INIT_2__(GABS_DYN_ALLOC_IMPL(impl_, alloc),             \
                                GABS_DYN_ALLOC_IMPL(impl_, dealloc))

#define GABS_DYN_ALLOC_INIT__(count_, ...)                                     \
        GABS_CONCAT_3(GABS_DYN_ALLOC_INIT_, count_, __)(__VA_ARGS__)

#define GABS_DYN_ALLOC_INIT(...)                                               \
        GABS_DYN_ALLOC_INIT__(GABS_COUNT_VA_ARGS(__VA_ARGS__), __VA_ARGS__)

#define GABS_DYN_ALLOC_HANDLE(alloc_) (&(alloc_)->handle)

#define GABS_DYN_ALLOC_GET(handle_)                                            \
        gabs_container_of(handle_, struct gabs_dyn_allocator, handle)

GABS_END_DECL

#endif /* GABS_ALLOC_DYNAMIC_H__ */
