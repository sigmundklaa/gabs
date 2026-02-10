
#include <string.h>

#include <gabs/pbuf.h>
#include <gabs/core/util.h>

size_t gabs_pbuf_size(gabs_pbuf buf)
{
        gabs_pbuf_ci it;
        size_t size;

        size = 0;

        gabs_pbuf_ci_foreach(&buf, it)
        {
                size += gabs_pbuf_ci_size(it);
        }

        return size;
}

void gabs_pbuf_chain_at(gabs_pbuf *buf, gabs_pbuf next, size_t offset)
{
        gabs_pbuf_ci it;
        size_t bytes;

        bytes = 0;

        if (offset == 0) {
                gabs_pbuf_chain_front(buf, next);
                return;
        }

        gabs_pbuf_ci_foreach(buf, it)
        {
                if (bytes + gabs_pbuf_ci_size(it) > offset) {
                        break;
                }

                bytes += gabs_pbuf_ci_size(it);
        }

        assert(bytes == offset);

        (void)gabs_pbuf_ci_insert(it, next);
}

void gabs_pbuf_chain_back(gabs_pbuf *buf, gabs_pbuf back)
{
        gabs_pbuf_ci it;

        gabs_pbuf_ci_foreach(buf, it){}

        (void)gabs_pbuf_ci_insert(it, back);
}

void gabs_pbuf_chain_front(gabs_pbuf *buf, gabs_pbuf front)
{
        gabs_pbuf_ci it;

        it = gabs_pbuf_ci_init(buf);

        (void)gabs_pbuf_ci_insert(it, front);
}

static gabs_pbuf_ci buf_seek(gabs_pbuf *buf, size_t offset, size_t *boundary)
{
        size_t bytes;
        gabs_pbuf_ci it;

        bytes = 0;

        gabs_pbuf_ci_foreach(buf, it)
        {
                if (offset == 0 || bytes + gabs_pbuf_ci_size(it) > offset) {
                        break;
                }

                bytes += gabs_pbuf_ci_size(it);
        }

        if (boundary != NULL) {
                *boundary = bytes;
        }

        return it;
}

gabs_pbuf gabs_pbuf_view(gabs_pbuf buf, size_t offset, size_t size,
                         const gabs_allocator_h *alloc)
{
        gabs_pbuf_ci it;
        gabs_pbuf_ci inserter;
        size_t boundary;
        size_t cur_offset;
        size_t cur_size;
        gabs_pbuf view;
        gabs_pbuf cur;

        it = buf_seek(&buf, offset, &boundary);

        cur_offset = offset - boundary;
        cur_size = gabs_min(gabs_pbuf_ci_size(it) - cur_offset, size);

        view = gabs_pbuf_new_ro(alloc, gabs_pbuf_ci_data(it) + cur_offset,
                                cur_size);
        if (!gabs_pbuf_okay(view)) {
                return view;
        }

        inserter = gabs_pbuf_ci_init(&view);
        size -= cur_size;

        while (size > 0) {
                it = gabs_pbuf_ci_next(it);
                cur_size = gabs_min(gabs_pbuf_ci_size(it), size);
                size -= cur_size;

                cur = gabs_pbuf_new_ro(alloc, gabs_pbuf_ci_data(it), cur_size);
                if (!gabs_pbuf_okay(cur)) {
                        assert(0);

                        break;
                }

                inserter =
                        gabs_pbuf_ci_insert(gabs_pbuf_ci_next(inserter), cur);
        }

        return view;
}

size_t gabs_pbuf_copy(gabs_pbuf buf, uint8_t *mem, size_t offset,
                      size_t max_size)
{
        gabs_pbuf_ci it;
        size_t boundary;
        size_t cur_offset;
        size_t cur_size;
        size_t size;

        size = 0;
        it = buf_seek(&buf, offset, &boundary);

        if (!gabs_pbuf_okay(buf)) {
                return 0;
        }

        cur_offset = offset - boundary;
        cur_size = gabs_min(gabs_pbuf_ci_size(it) - cur_offset, max_size);

        (void)memcpy(mem, gabs_pbuf_ci_data(it) + cur_offset, cur_size);

        mem += cur_size;
        size += cur_size;

        while (size < max_size) {
                it = gabs_pbuf_ci_next(it);
                if (gabs_pbuf_ci_eoi(it)) {
                        break;
                }

                cur_size = gabs_min(gabs_pbuf_ci_size(it), max_size - size);

                (void)memcpy(mem, gabs_pbuf_ci_data(it) + cur_offset, cur_size);

                mem += cur_size;
                size += cur_size;
        }

        return size;
}

gabs_pbuf gabs_pbuf_clone(gabs_pbuf buf, size_t offset, size_t size,
                          const gabs_allocator_h *alloc)
{
        size_t bytes;
        gabs_pbuf ret;

        ret = gabs_pbuf_new(alloc, size);
        if (!gabs_pbuf_okay(ret)) {
                return ret;
        }

        bytes = gabs_pbuf_copy(
                buf, gabs_pbuf_ci_reserve_tail(gabs_pbuf_ci_init(&ret), size),
                offset, size);
        assert(bytes == size);

        return ret;
}

void gabs_pbuf_shrink(gabs_pbuf *buf, size_t offset, size_t size)
{
        gabs_pbuf_ci it;
        size_t bytes;
        size_t rem;

        it = gabs_pbuf_ci_init(buf);

        if (offset > 0) {
                bytes = 0;

                while (!gabs_pbuf_ci_eoi(it)) {
                        if (bytes + gabs_pbuf_ci_size(it) > offset) {
                                break;
                        }

                        bytes += gabs_pbuf_ci_size(it);
                        it = gabs_pbuf_ci_remove(it);
                }

                if (gabs_pbuf_ci_eoi(it)) {
                        return;
                }

                if (offset > bytes) {
                        (void)gabs_pbuf_ci_release_head(it, offset - bytes);
                }
        }

        {
                bytes = 0;
                rem = size;

                /* Iterate over `size` bytes, and remove anything after those
                 * bytes. */
                for (; !gabs_pbuf_ci_eoi(it); it = gabs_pbuf_ci_next(it)) {
                        bytes = gabs_min(gabs_pbuf_ci_size(it), rem);
                        rem -= bytes;

                        if (rem == 0) {
                                break;
                        }
                }

                if (!gabs_pbuf_ci_eoi(it)) {
                        if (gabs_pbuf_ci_size(it) > bytes) {
                                (void)gabs_pbuf_ci_release_tail(
                                        it, gabs_pbuf_ci_size(it) - bytes);
                        }

                        it = gabs_pbuf_ci_next(it);
                        if (!gabs_pbuf_ci_eoi(it)) {
                                gabs_pbuf_ci_detach(it);
                        }
                }
        }
}

void gabs_pbuf_strip_head(gabs_pbuf *buf, size_t size)
{
        gabs_pbuf_shrink(buf, size, gabs_pbuf_size(*buf) - size);
}

void gabs_pbuf_strip_tail(gabs_pbuf *buf, size_t size)
{
        gabs_pbuf_shrink(buf, 0, gabs_pbuf_size(*buf) - size);
}

void gabs_pbuf_put(gabs_pbuf *buf, const uint8_t *data, size_t size)
{
        gabs_pbuf_ci it;
        size_t bytes;
        uint8_t *mem;

        it = gabs_pbuf_ci_init(buf);

        for (; size > 0 && !gabs_pbuf_ci_eoi(it); it = gabs_pbuf_ci_next(it)) {
                bytes = gabs_min(gabs_pbuf_ci_tailroom(it), size);

                mem = gabs_pbuf_ci_reserve_tail(it, bytes);
                assert(mem != NULL);

                (void)memcpy(mem, data, bytes);
                data += bytes;
                size -= bytes;
        }
}

size_t gabs_pbuf_tailroom(gabs_pbuf buf)
{
        gabs_pbuf_ci it;

        gabs_pbuf_ci_foreach(&buf, it)
        {
                if (gabs_pbuf_ci_eoi(gabs_pbuf_ci_next(it))) {
                        break;
                }
        }

        if (gabs_pbuf_ci_eoi(it)) {
                return 0;
        }

        return gabs_pbuf_ci_tailroom(it);
}

size_t gabs_pbuf_headroom(gabs_pbuf buf)
{
        gabs_pbuf_ci it;

        it = gabs_pbuf_ci_init(&buf);
        if (gabs_pbuf_ci_eoi(it)) {
                return 0;
        }

        return gabs_pbuf_ci_headroom(it);
}
