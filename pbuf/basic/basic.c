
#include <stdalign.h>

#define GABS_PBUF_INCLUDE_DEF
#include <gabs/pbuf.h>
#include "gabs_pbuf_def.h"

struct gabs_pbuf_basic GABS_PBUF_IMPL(new)(const gabs_allocator_h *alloc,
                                           size_t size)
{
        struct gabs_pbuf_basic_frag *buf;
        int status;

        status = gabs_alloc(alloc, sizeof(*buf) + size, (void **)&buf);
        if (status != 0) {
                assert(0);
                return (struct gabs_pbuf_basic){0};
        }

        buf->cap = size;
        buf->size = 0;
        buf->data = (uint8_t *)buf + sizeof(*buf);
        buf->next = NULL;
        buf->refcnt = 1;
        buf->readonly = false;
        buf->alloc = alloc;

        return (struct gabs_pbuf_basic){.frags = buf};
}

struct gabs_pbuf_basic GABS_PBUF_IMPL(new_ro)(const gabs_allocator_h *alloc,
                                              uint8_t *data, size_t size)
{
        struct gabs_pbuf_basic buf;
        struct gabs_pbuf_basic_frag *frag;

        buf = gabs_pbuf_new(alloc, 0);
        if (!gabs_pbuf_okay(buf)) {
                assert(0);
                return (struct gabs_pbuf_basic){0};
        }

        frag = buf.frags;

        frag->readonly = true;
        frag->data = data;
        frag->size = size;
        frag->cap = size;

        return buf;
}

bool GABS_PBUF_IMPL(okay)(struct gabs_pbuf_basic buf)
{
        return buf.frags != NULL;
}

void GABS_PBUF_IMPL(incref)(struct gabs_pbuf_basic buf)
{
        buf.frags->refcnt++;
}

void GABS_PBUF_IMPL(decref)(struct gabs_pbuf_basic buf)
{
        struct gabs_pbuf_basic_frag *cur;
        struct gabs_pbuf_basic_frag *next;

        for (cur = buf.frags; cur != NULL; cur = next) {
                next = cur->next;

                if (--cur->refcnt > 0) {
                        break;
                }

                gabs_dealloc(cur->alloc, cur);
        }
}

struct gabs_pbuf_basic_ci GABS_PBUF_IMPL(ci_init)(struct gabs_pbuf_basic *buf)
{
        return (struct gabs_pbuf_basic_ci){
                .cur = buf->frags,
                .lastp = &buf->frags,
        };
}

struct gabs_pbuf_basic_ci GABS_PBUF_IMPL(ci_next)(struct gabs_pbuf_basic_ci it)
{
        return (struct gabs_pbuf_basic_ci){
                .cur = it.cur->next,
                .lastp = &it.cur->next,
        };
}

bool GABS_PBUF_IMPL(ci_eoi)(struct gabs_pbuf_basic_ci it)
{
        return it.cur == NULL;
}

uint8_t *GABS_PBUF_IMPL(ci_data)(struct gabs_pbuf_basic_ci it)
{
        return it.cur->data;
}

uint8_t *GABS_PBUF_IMPL(ci_reserve_head)(struct gabs_pbuf_basic_ci it,
                                         size_t bytes)
{
        uint8_t *data;
        assert(bytes <= gabs_pbuf_ci_headroom(it));

        data = it.cur->data - bytes;
        it.cur->size += bytes;

        return data;
}

uint8_t *GABS_PBUF_IMPL(ci_release_head)(struct gabs_pbuf_basic_ci it,
                                         size_t bytes)
{
        uint8_t *data;

        assert(bytes <= it.cur->size);

        data = it.cur->data;
        it.cur->size -= bytes;
        it.cur->data += bytes;

        return data;
}

uint8_t *GABS_PBUF_IMPL(ci_reserve_tail)(struct gabs_pbuf_basic_ci it,
                                         size_t bytes)
{
        uint8_t *data;

        assert(bytes <= gabs_pbuf_ci_tailroom(it));

        data = it.cur->data + it.cur->size;
        it.cur->size += bytes;

        return data;
}

uint8_t *GABS_PBUF_IMPL(ci_release_tail)(struct gabs_pbuf_basic_ci it,
                                         size_t bytes)
{
        assert(bytes <= it.cur->size);

        it.cur->size -= bytes;

        return it.cur->data + it.cur->size;
}

size_t GABS_PBUF_IMPL(ci_headroom)(struct gabs_pbuf_basic_ci it)
{
        uint8_t *base;

        if (it.cur->readonly) {
                base = it.cur->data;
        } else {
                base = gabs_pbuf_ci_data(it);
        }

        return it.cur->data - base;
}

size_t GABS_PBUF_IMPL(ci_tailroom)(struct gabs_pbuf_basic_ci it)
{
        return it.cur->cap - it.cur->size - gabs_pbuf_ci_headroom(it);
}

struct gabs_pbuf_basic_ci
GABS_PBUF_IMPL(ci_insert)(struct gabs_pbuf_basic_ci it,
                          struct gabs_pbuf_basic buf)
{
        struct gabs_pbuf_basic_frag **lastp;
        struct gabs_pbuf_basic_frag *cur;

        lastp = &buf.frags;

        for (cur = buf.frags; cur != NULL; cur = cur->next) {
                lastp = &cur->next;
        }

        *lastp = it.cur;
        *it.lastp = buf.frags;

        return (struct gabs_pbuf_basic_ci){
                .cur = buf.frags,
                .lastp = it.lastp,
        };
}

void GABS_PBUF_IMPL(ci_detach)(struct gabs_pbuf_basic_ci it)
{
        *it.lastp = NULL;

        gabs_pbuf_decref((struct gabs_pbuf_basic){it.cur});
}

struct gabs_pbuf_basic_ci
GABS_PBUF_IMPL(ci_remove)(struct gabs_pbuf_basic_ci it)
{
        struct gabs_pbuf_basic_frag *buf;

        buf = it.cur;
        it.cur = it.cur->next;
        *it.lastp = it.cur;

        buf->next = NULL;

        gabs_pbuf_decref((struct gabs_pbuf_basic){buf});

        return it;
}

size_t GABS_PBUF_IMPL(ci_size)(struct gabs_pbuf_basic_ci it)
{
        return it.cur->size;
}

size_t GABS_PBUF_IMPL(ci_cap)(struct gabs_pbuf_basic_ci it)
{
        return it.cur->cap;
}
