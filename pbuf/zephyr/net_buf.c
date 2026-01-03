
#include <zephyr/net_buf.h>

#include <gabs/pbuf.h>

struct gabs_pbuf_zephyr gabs_pbuf_new(const gabs_allocator_h *alloc,
                                      size_t size)
{
        struct net_buf *buf;

        buf = gabs_alloc(alloc, size);
        if (buf == NULL) {
                assert(0);
                return (struct gabs_pbuf_zephyr){0};
        }

        return gabs_pbuf_from_zephyr(buf);
}

struct gabs_pbuf_zephyr gabs_pbuf_new_ro(const gabs_allocator_h *alloc,
                                         uint8_t *data, size_t size)
{
        struct gabs_pbuf_zephyr buf;

        buf = gabs_pbuf_new(alloc, 0);
        if (!gabs_pbuf_okay(buf)) {
                assert(0);
                return (struct gabs_pbuf_zephyr){0};
        }

        net_buf_simple_init_with_data(&buf.frags->b, data, size);
        buf.frags->flags = NET_BUF_EXTERNAL_DATA;

        return buf;
}

bool gabs_pbuf_okay(struct gabs_pbuf_zephyr buf)
{
        return buf.frags != NULL;
}

void gabs_pbuf_incref(struct gabs_pbuf_zephyr buf)
{
        buf.frags = net_buf_ref(buf.frags);
}

void gabs_pbuf_decref(struct gabs_pbuf_zephyr buf)
{
        if (buf.frags != NULL) {
                net_buf_unref(buf.frags);
        }
}

struct gabs_pbuf_zephyr_ci gabs_pbuf_ci_init(struct gabs_pbuf_zephyr *buf)
{
        return (struct gabs_pbuf_zephyr_ci){
                .cur = buf->frags,
                .prev = NULL,
                .head = &buf->frags,
        };
}

struct gabs_pbuf_zephyr_ci gabs_pbuf_ci_next(struct gabs_pbuf_zephyr_ci it)
{
        return (struct gabs_pbuf_zephyr_ci){
                .cur = it.cur->frags,
                .prev = it.cur,
                .head = it.head,
        };
}

bool gabs_pbuf_ci_eoi(struct gabs_pbuf_zephyr_ci it)
{
        return it.cur == NULL;
}

uint8_t *gabs_pbuf_ci_data(struct gabs_pbuf_zephyr_ci it)
{
        return it.cur->data;
}

uint8_t *gabs_pbuf_ci_reserve_head(struct gabs_pbuf_zephyr_ci it, size_t bytes)
{
        return net_buf_push(it.cur, bytes);
}

uint8_t *gabs_pbuf_ci_release_head(struct gabs_pbuf_zephyr_ci it, size_t bytes)
{
        return net_buf_pull_mem(it.cur, bytes);
}

uint8_t *gabs_pbuf_ci_reserve_tail(struct gabs_pbuf_zephyr_ci it, size_t bytes)
{
        return net_buf_add(it.cur, bytes);
}

uint8_t *gabs_pbuf_ci_release_tail(struct gabs_pbuf_zephyr_ci it, size_t bytes)
{
        return net_buf_remove_mem(it.cur, bytes);
}

size_t gabs_pbuf_ci_headroom(struct gabs_pbuf_zephyr_ci it)
{
        return net_buf_headroom(it.cur);
}

size_t gabs_pbuf_ci_tailroom(struct gabs_pbuf_zephyr_ci it)
{
        return net_buf_tailroom(it.cur);
}

struct gabs_pbuf_zephyr_ci gabs_pbuf_ci_insert(struct gabs_pbuf_zephyr_ci it,
                                               struct gabs_pbuf_zephyr buf)
{
        struct net_buf *cur;

        if (it.prev != NULL) {
                net_buf_frag_insert(it.prev, buf.frags);
                cur = buf.frags;
        } else {
                if (it.cur != NULL) {
                        cur = net_buf_frag_add(buf.frags, it.cur);
                } else {
                        cur = buf.frags;
                }

                *it.head = cur;
        }

        it.cur = buf.frags;
        return it;
}

void gabs_pbuf_ci_detach(struct gabs_pbuf_zephyr_ci it)
{
        struct net_buf *frag = it.cur;

        if (*it.head == it.cur) {
                *it.head = NULL;
        }

        while (frag != NULL) {
                frag = net_buf_frag_del(NULL, it.cur);
        }
}

struct gabs_pbuf_zephyr_ci gabs_pbuf_ci_remove(struct gabs_pbuf_zephyr_ci it)
{
        bool is_head;

        is_head = *it.head == it.cur;
        it.cur = net_buf_frag_del(it.prev, it.cur);

        if (is_head) {
                *it.head = it.cur;
        }

        return it;
}

size_t gabs_pbuf_ci_size(struct gabs_pbuf_zephyr_ci it)
{
        return it.cur->len;
}
