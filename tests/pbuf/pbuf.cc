
#include <string.h>

#include <gabs/testing/testing.h>

#include <gabs/pbuf.h>

#include "pbuf_alloc.h"

namespace
{

constexpr size_t buf_size = 32;

const uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
constexpr size_t half = sizeof(data) / 2;

class leak_fixture
{
      public:
        ~leak_fixture()
        {
                GABS_EXPECT_EQ(gabs_test_pbuf_outstanding(), 0u);
        }
};

gabs_pbuf new_with(const uint8_t *mem, size_t size)
{
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), buf_size);

        GABS_ASSERT_TRUE(gabs_pbuf_okay(buf));
        gabs_pbuf_put(&buf, mem, size);

        return buf;
}

/* Two fragments, holding the first and second half of `data`. */
gabs_pbuf new_chain()
{
        gabs_pbuf buf = new_with(data, half);

        gabs_pbuf_chain_back(&buf, new_with(data + half, half));

        return buf;
}

bool holds(gabs_pbuf buf, const uint8_t *expected, size_t size)
{
        uint8_t out[buf_size] = {};

        return gabs_pbuf_size(buf) == size &&
               gabs_pbuf_copy(buf, out, 0, sizeof(out)) == size &&
               memcmp(out, expected, size) == 0;
}

} // namespace

GABS_SUITE_F(pbuf, leak_fixture);

GABS_TEST_F(pbuf, new_is_okay_and_empty)
{
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), buf_size);
        bool okay = gabs_pbuf_okay(buf);
        size_t size = gabs_pbuf_size(buf);

        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(okay);
        GABS_ASSERT_EQ(size, 0u);
}

GABS_TEST_F(pbuf, new_has_requested_room)
{
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), buf_size);
        gabs_pbuf_ci it = gabs_pbuf_ci_init(&buf);
        size_t cap = gabs_pbuf_ci_cap(it);
        size_t tailroom = gabs_pbuf_tailroom(buf);

        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(cap >= buf_size);
        GABS_ASSERT_TRUE(tailroom >= buf_size);
}

GABS_TEST_F(pbuf, put_then_copy)
{
        gabs_pbuf buf = new_with(data, sizeof(data));
        bool ok = holds(buf, data, sizeof(data));

        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, put_takes_tailroom)
{
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), buf_size);
        size_t before = gabs_pbuf_tailroom(buf);
        size_t after;

        gabs_pbuf_put(&buf, data, sizeof(data));
        after = gabs_pbuf_tailroom(buf);
        gabs_pbuf_decref(buf);

        GABS_ASSERT_EQ(after, before - sizeof(data));
}

GABS_TEST_F(pbuf, new_ro_exposes_data)
{
        uint8_t src[] = {9, 8, 7, 6};
        gabs_pbuf buf =
                gabs_pbuf_new_ro(gabs_test_pbuf_alloc(), src, sizeof(src));
        bool okay = gabs_pbuf_okay(buf);
        bool ok = okay && holds(buf, src, sizeof(src));

        if (okay) {
                gabs_pbuf_decref(buf);
        }

        GABS_ASSERT_TRUE(okay);
        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, incref_keeps_buffer_after_decref)
{
        gabs_pbuf buf = new_with(data, sizeof(data));
        bool ok;

        gabs_pbuf_incref(buf);
        gabs_pbuf_decref(buf);
        ok = holds(buf, data, sizeof(data));
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, strip_head)
{
        gabs_pbuf buf = new_with(data, sizeof(data));
        bool ok;

        gabs_pbuf_strip_head(&buf, 3);
        ok = holds(buf, data + 3, sizeof(data) - 3);
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, strip_tail)
{
        gabs_pbuf buf = new_with(data, sizeof(data));
        bool ok;

        gabs_pbuf_strip_tail(&buf, 3);
        ok = holds(buf, data, sizeof(data) - 3);
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, strip_head_across_fragments)
{
        gabs_pbuf buf = new_chain();
        bool ok;

        gabs_pbuf_strip_head(&buf, half + 1);
        ok = holds(buf, data + half + 1, half - 1);
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, strip_tail_across_fragments)
{
        gabs_pbuf buf = new_chain();
        bool ok;

        gabs_pbuf_strip_tail(&buf, half + 1);
        ok = holds(buf, data, half - 1);
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, chain_back)
{
        gabs_pbuf buf = new_chain();
        bool ok = holds(buf, data, sizeof(data));

        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, chain_front)
{
        gabs_pbuf buf = new_with(data + half, half);
        bool ok;

        gabs_pbuf_chain_front(&buf, new_with(data, half));
        ok = holds(buf, data, sizeof(data));
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, chain_at_fragment_boundary)
{
        const uint8_t expected[] = {0, 1, 2, 3, 9, 4, 5, 6, 7};
        const uint8_t middle[] = {9};
        gabs_pbuf buf = new_chain();
        bool ok;

        gabs_pbuf_chain_at(&buf, new_with(middle, sizeof(middle)), half);
        ok = holds(buf, expected, sizeof(expected));
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, copy_from_offset_across_fragments)
{
        uint8_t out[sizeof(data)] = {};
        gabs_pbuf buf = new_chain();
        size_t copied = gabs_pbuf_copy(buf, out, 2, sizeof(out));

        gabs_pbuf_decref(buf);

        GABS_ASSERT_EQ(copied, sizeof(data) - 2);
        GABS_ASSERT_EQ(memcmp(out, data + 2, copied), 0);
}

GABS_TEST_F(pbuf, copy_from_offset_in_second_fragment)
{
        uint8_t out[sizeof(data)] = {};
        gabs_pbuf buf = new_chain();
        size_t copied = gabs_pbuf_copy(buf, out, half + 1, sizeof(out));

        gabs_pbuf_decref(buf);

        GABS_ASSERT_EQ(copied, half - 1);
        GABS_ASSERT_EQ(memcmp(out, data + half + 1, copied), 0);
}

GABS_TEST_F(pbuf, copy_stops_at_max_size)
{
        uint8_t out[sizeof(data)] = {};
        gabs_pbuf buf = new_chain();
        size_t copied = gabs_pbuf_copy(buf, out, 1, half);

        gabs_pbuf_decref(buf);

        GABS_ASSERT_EQ(copied, half);
        GABS_ASSERT_EQ(memcmp(out, data + 1, half), 0);
        GABS_ASSERT_EQ(out[half], 0);
}

GABS_TEST_F(pbuf, view_of_subrange)
{
        gabs_pbuf buf = new_with(data, sizeof(data));
        gabs_pbuf view = gabs_pbuf_view(buf, 2, half, gabs_test_pbuf_alloc());
        bool okay = gabs_pbuf_okay(view);
        bool ok = okay && holds(view, data + 2, half);

        if (okay) {
                gabs_pbuf_decref(view);
        }
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(okay);
        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, view_across_fragments)
{
        gabs_pbuf buf = new_chain();
        gabs_pbuf view = gabs_pbuf_view(buf, 2, half, gabs_test_pbuf_alloc());
        bool okay = gabs_pbuf_okay(view);
        bool ok = okay && holds(view, data + 2, half);

        if (okay) {
                gabs_pbuf_decref(view);
        }
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(okay);
        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, view_shares_data)
{
        const uint8_t expected[] = {2, 42, 4, 5};
        gabs_pbuf buf = new_with(data, sizeof(data));
        gabs_pbuf view = gabs_pbuf_view(buf, 2, half, gabs_test_pbuf_alloc());
        bool okay = gabs_pbuf_okay(view);
        bool ok;

        gabs_pbuf_ci_data(gabs_pbuf_ci_init(&buf))[3] = 42;
        ok = okay && holds(view, expected, sizeof(expected));

        if (okay) {
                gabs_pbuf_decref(view);
        }
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(okay);
        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, clone_of_subrange)
{
        gabs_pbuf buf = new_chain();
        gabs_pbuf clone = gabs_pbuf_clone(buf, 2, half, gabs_test_pbuf_alloc());
        bool okay = gabs_pbuf_okay(clone);
        bool ok = okay && holds(clone, data + 2, half);

        if (okay) {
                gabs_pbuf_decref(clone);
        }
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(okay);
        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, clone_does_not_share_data)
{
        gabs_pbuf buf = new_with(data, sizeof(data));
        gabs_pbuf clone = gabs_pbuf_clone(buf, 2, half, gabs_test_pbuf_alloc());
        bool okay = gabs_pbuf_okay(clone);
        bool ok;

        gabs_pbuf_ci_data(gabs_pbuf_ci_init(&buf))[3] = 42;
        ok = okay && holds(clone, data + 2, half);

        if (okay) {
                gabs_pbuf_decref(clone);
        }
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(okay);
        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, headroom_and_tailroom_of_chain)
{
        gabs_pbuf buf = new_chain();
        gabs_pbuf_ci first = gabs_pbuf_ci_init(&buf);
        gabs_pbuf_ci last = gabs_pbuf_ci_next(first);
        size_t headroom = gabs_pbuf_headroom(buf);
        size_t tailroom = gabs_pbuf_tailroom(buf);
        size_t first_headroom = gabs_pbuf_ci_headroom(first);
        size_t last_tailroom = gabs_pbuf_ci_tailroom(last);

        gabs_pbuf_decref(buf);

        GABS_ASSERT_EQ(headroom, first_headroom);
        GABS_ASSERT_EQ(tailroom, last_tailroom);
}

GABS_TEST_F(pbuf, ci_foreach_visits_every_fragment)
{
        gabs_pbuf buf = new_chain();
        gabs_pbuf_ci it;
        int fragments = 0;
        size_t total = 0;

        gabs_pbuf_ci_foreach(&buf, it)
        {
                fragments++;
                total += gabs_pbuf_ci_size(it);
        }

        gabs_pbuf_decref(buf);

        GABS_ASSERT_EQ(fragments, 2);
        GABS_ASSERT_EQ(total, sizeof(data));
}

GABS_TEST_F(pbuf, ci_reserve_and_release_tail)
{
        gabs_pbuf buf = new_with(data, half);
        gabs_pbuf_ci it = gabs_pbuf_ci_init(&buf);
        size_t tailroom = gabs_pbuf_ci_tailroom(it);
        uint8_t *tail = gabs_pbuf_ci_data(it) + half;
        uint8_t *reserved = gabs_pbuf_ci_reserve_tail(it, 5);
        size_t reserved_tailroom = gabs_pbuf_ci_tailroom(it);
        size_t reserved_size = gabs_pbuf_ci_size(it);
        uint8_t *released = gabs_pbuf_ci_release_tail(it, 5);
        size_t released_tailroom = gabs_pbuf_ci_tailroom(it);

        gabs_pbuf_decref(buf);

        GABS_ASSERT_EQ(reserved, tail);
        GABS_ASSERT_EQ(reserved_tailroom, tailroom - 5);
        GABS_ASSERT_EQ(reserved_size, half + 5);
        GABS_ASSERT_EQ(released, tail);
        GABS_ASSERT_EQ(released_tailroom, tailroom);
}

GABS_TEST_F(pbuf, ci_release_and_reserve_head)
{
        gabs_pbuf buf = new_with(data, sizeof(data));
        gabs_pbuf_ci it = gabs_pbuf_ci_init(&buf);
        size_t headroom = gabs_pbuf_ci_headroom(it);
        uint8_t *head = gabs_pbuf_ci_data(it);
        uint8_t *released = gabs_pbuf_ci_release_head(it, 3);
        size_t released_headroom = gabs_pbuf_ci_headroom(it);
        uint8_t *released_data = gabs_pbuf_ci_data(it);
        uint8_t *reserved = gabs_pbuf_ci_reserve_head(it, 3);
        size_t reserved_headroom = gabs_pbuf_ci_headroom(it);
        bool ok = holds(buf, data, sizeof(data));

        gabs_pbuf_decref(buf);

        GABS_ASSERT_EQ(released, head);
        GABS_ASSERT_EQ(released_headroom, headroom + 3);
        GABS_ASSERT_EQ(released_data, head + 3);
        GABS_ASSERT_EQ(reserved, head);
        GABS_ASSERT_EQ(reserved_headroom, headroom);
        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, ci_release_head_keeps_tailroom)
{
        gabs_pbuf buf = new_with(data, sizeof(data));
        gabs_pbuf_ci it = gabs_pbuf_ci_init(&buf);
        size_t tailroom = gabs_pbuf_ci_tailroom(it);
        size_t released_tailroom;

        (void)gabs_pbuf_ci_release_head(it, 3);
        released_tailroom = gabs_pbuf_ci_tailroom(it);
        gabs_pbuf_decref(buf);

        GABS_ASSERT_EQ(released_tailroom, tailroom);
}

GABS_TEST_F(pbuf, ci_remove_first_fragment)
{
        gabs_pbuf buf = new_chain();
        gabs_pbuf_ci it = gabs_pbuf_ci_init(&buf);
        bool ok;

        it = gabs_pbuf_ci_remove(it);
        ok = !gabs_pbuf_ci_eoi(it) && gabs_pbuf_ci_size(it) == half &&
             holds(buf, data + half, half);
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, ci_remove_last_fragment)
{
        gabs_pbuf buf = new_chain();
        gabs_pbuf_ci it = gabs_pbuf_ci_next(gabs_pbuf_ci_init(&buf));
        bool ok;

        it = gabs_pbuf_ci_remove(it);
        ok = gabs_pbuf_ci_eoi(it) && holds(buf, data, half);
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}

GABS_TEST_F(pbuf, ci_detach_drops_following_fragments)
{
        gabs_pbuf buf = new_chain();
        bool ok;

        gabs_pbuf_chain_back(&buf, new_with(data, half));
        gabs_pbuf_ci_detach(gabs_pbuf_ci_next(gabs_pbuf_ci_init(&buf)));
        ok = holds(buf, data, half);
        gabs_pbuf_decref(buf);

        GABS_ASSERT_TRUE(ok);
}
