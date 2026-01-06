
#ifndef GABS_PBUF_BASIC_DEF_H__
#define GABS_PBUF_BASIC_DEF_H__

#ifndef GABS_PBUF_INCLUDE_DEF
#error "gabs_pbuf_def.h should not be included directly"
#endif /* GABS_PBUF_INCLUDE_DEF */

#include <stddef.h>
#include <stdbool.h>

#include <gabs/core/util.h>
#include <gabs/alloc.h>

GABS_BEGIN_DECL

struct gabs_pbuf_basic_frag {
        uint8_t *data;
        size_t size;
        size_t cap;

        bool readonly;
        unsigned int refcnt;

        struct gabs_pbuf_basic_frag *next;
        const gabs_allocator_h *alloc;
};

struct gabs_pbuf_basic {
        struct gabs_pbuf_basic_frag *frags;
};

struct gabs_pbuf_basic_ci {
        struct gabs_pbuf_basic_frag **lastp;
        struct gabs_pbuf_basic_frag *cur;
};

typedef struct gabs_pbuf_basic gabs_pbuf;
typedef struct gabs_pbuf_basic_ci gabs_pbuf_ci;

GABS_END_DECL

#endif /* GABS_PBUF_BASIC_DEF_H__ */
