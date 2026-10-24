
#ifndef GABS_ALLOC_H__
#define GABS_ALLOC_H__

#include <stdbool.h>
#include <stddef.h>

#include <gabs/core/api.h>
#include <gabs/core/handle.h>
#include <gabs/core/util.h>

GABS_BEGIN_DECL

typedef gabs_handle gabs_allocator_h;

typedef int (*gabs_allocator_alloc_fn)(const gabs_allocator_h *, size_t,
                                       void **);
typedef int (*gabs_allocator_dealloc_fn)(const gabs_allocator_h *, void *);

struct gabs_allocator_api {
        gabs_allocator_alloc_fn alloc;
        gabs_allocator_dealloc_fn dealloc;
};

struct gabs_allocator {
        const struct gabs_allocator_api *api;
        gabs_allocator_h handle;
};

typedef struct gabs_allocator gabs_allocator;

#define GABS_ALLOCATOR_INIT(api_)                                              \
        {                                                                      \
                .api = (api_),                                                 \
                .handle = {0},                                                 \
        }

#define GABS_ALLOCATOR_HANDLE(allocator_) (&(allocator_)->handle)

#define GABS_ALLOCATOR_GET(handle_)                                            \
        gabs_container_of(handle_, struct gabs_allocator, handle)

GABS_API int gabs_alloc(const gabs_allocator_h *allocator, size_t size,
                        void **mem);
GABS_API int gabs_dealloc(const gabs_allocator_h *allocator, void *mem);
GABS_API bool gabs_allocator_is(const gabs_allocator_h *allocator,
                                const struct gabs_allocator_api *api);

GABS_END_DECL

#endif /* GABS_ALLOC_H__ */
