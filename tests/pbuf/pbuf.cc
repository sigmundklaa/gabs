
#include <cstring>

#include <gabs/testing/testing.h>

#include <gabs/pbuf.h>

#include "pbuf_alloc.h"

GABS_SUITE(pbuf);

GABS_TEST(pbuf, new_buffer_is_okay_and_empty)
{
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 32);

        GABS_ASSERT_TRUE(gabs_pbuf_okay(buf));
        GABS_ASSERT_EQ(gabs_pbuf_size(buf), 0u);

        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, put_then_size_reflects_bytes_written)
{
        static const uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8};
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 32);

        gabs_pbuf_put(&buf, data, sizeof(data));

        GABS_ASSERT_EQ(gabs_pbuf_size(buf), sizeof(data));

        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, put_then_copy_roundtrips_bytes)
{
        static const uint8_t data[] = {10, 20, 30, 40, 50};
        uint8_t out[sizeof(data)] = {};
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 32);
        size_t copied;

        gabs_pbuf_put(&buf, data, sizeof(data));
        copied = gabs_pbuf_copy(buf, out, 0, sizeof(out));

        GABS_ASSERT_EQ(copied, sizeof(out));
        GABS_ASSERT_EQ(memcmp(data, out, sizeof(data)), 0);

        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, new_ro_view_exposes_underlying_data)
{
        static uint8_t src[] = {9, 8, 7, 6};
        uint8_t out[sizeof(src)] = {};
        gabs_pbuf buf = gabs_pbuf_new_ro(gabs_test_pbuf_alloc(), src, sizeof(src));
        size_t copied;

        GABS_ASSERT_TRUE(gabs_pbuf_okay(buf));
        GABS_ASSERT_EQ(gabs_pbuf_size(buf), sizeof(src));

        copied = gabs_pbuf_copy(buf, out, 0, sizeof(out));
        GABS_ASSERT_EQ(copied, sizeof(out));
        GABS_ASSERT_EQ(memcmp(src, out, sizeof(src)), 0);

        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, strip_head_removes_leading_bytes)
{
        static const uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
        uint8_t out[5] = {};
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 32);
        size_t copied;

        gabs_pbuf_put(&buf, data, sizeof(data));
        gabs_pbuf_strip_head(&buf, 3);

        GABS_ASSERT_EQ(gabs_pbuf_size(buf), sizeof(out));

        copied = gabs_pbuf_copy(buf, out, 0, sizeof(out));
        GABS_ASSERT_EQ(copied, sizeof(out));
        GABS_ASSERT_EQ(memcmp(data + 3, out, sizeof(out)), 0);

        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, strip_tail_removes_trailing_bytes)
{
        static const uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
        uint8_t out[5] = {};
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 32);
        size_t copied;

        gabs_pbuf_put(&buf, data, sizeof(data));
        gabs_pbuf_strip_tail(&buf, 3);

        GABS_ASSERT_EQ(gabs_pbuf_size(buf), sizeof(out));

        copied = gabs_pbuf_copy(buf, out, 0, sizeof(out));
        GABS_ASSERT_EQ(copied, sizeof(out));
        GABS_ASSERT_EQ(memcmp(data, out, sizeof(out)), 0);

        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, chain_back_combines_sizes)
{
        static const uint8_t first[] = {0, 1, 2, 3};
        static const uint8_t second[] = {4, 5, 6, 7};
        uint8_t out[8] = {};
        gabs_pbuf a = gabs_pbuf_new(gabs_test_pbuf_alloc(), sizeof(first));
        gabs_pbuf b = gabs_pbuf_new(gabs_test_pbuf_alloc(), sizeof(second));
        size_t copied;

        gabs_pbuf_put(&a, first, sizeof(first));
        gabs_pbuf_put(&b, second, sizeof(second));
        gabs_pbuf_chain_back(&a, b);

        GABS_ASSERT_EQ(gabs_pbuf_size(a), sizeof(out));

        copied = gabs_pbuf_copy(a, out, 0, sizeof(out));
        GABS_ASSERT_EQ(copied, sizeof(out));
        GABS_ASSERT_EQ(memcmp(first, out, sizeof(first)), 0);
        GABS_ASSERT_EQ(memcmp(second, out + sizeof(first), sizeof(second)), 0);

        /* Chaining transfers ownership of `b` into `a`'s fragment list, so a
         * single decref on the head releases the whole chain. */
        gabs_pbuf_decref(a);
}

GABS_TEST(pbuf, ci_foreach_visits_every_fragment)
{
        static const uint8_t data[4] = {1, 2, 3, 4};
        gabs_pbuf a = gabs_pbuf_new(gabs_test_pbuf_alloc(), sizeof(data));
        gabs_pbuf b = gabs_pbuf_new(gabs_test_pbuf_alloc(), sizeof(data));
        int fragments;
        size_t total;
        gabs_pbuf_ci it;

        gabs_pbuf_put(&a, data, sizeof(data));
        gabs_pbuf_put(&b, data, sizeof(data));
        gabs_pbuf_chain_back(&a, b);

        fragments = 0;
        total = 0;

        gabs_pbuf_ci_foreach(&a, it)
        {
                fragments++;
                total += gabs_pbuf_ci_size(it);
        }

        GABS_ASSERT_EQ(fragments, 2);
        GABS_ASSERT_EQ(total, 2 * sizeof(data));

        gabs_pbuf_decref(a);
}

GABS_TEST(pbuf, ci_reserve_tail_then_release_tail_is_symmetric)
{
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 32);
        gabs_pbuf_ci it = gabs_pbuf_ci_init(&buf);
        size_t tailroom_before = gabs_pbuf_ci_tailroom(it);

        GABS_ASSERT_NOT_NULL(gabs_pbuf_ci_reserve_tail(it, 5));
        GABS_ASSERT_EQ(gabs_pbuf_ci_tailroom(it), tailroom_before - 5);

        GABS_ASSERT_NOT_NULL(gabs_pbuf_ci_release_tail(it, 5));
        GABS_ASSERT_EQ(gabs_pbuf_ci_tailroom(it), tailroom_before);

        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, ci_release_head_then_reserve_head_is_symmetric)
{
        static const uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 32);
        gabs_pbuf_ci it;

        gabs_pbuf_put(&buf, data, sizeof(data));

        it = gabs_pbuf_ci_init(&buf);
        GABS_ASSERT_EQ(gabs_pbuf_ci_headroom(it), 0u);

        GABS_ASSERT_NOT_NULL(gabs_pbuf_ci_release_head(it, 3));
        GABS_ASSERT_EQ(gabs_pbuf_ci_headroom(it), 3u);
        GABS_ASSERT_EQ(gabs_pbuf_ci_size(it), sizeof(data) - 3);

        GABS_ASSERT_NOT_NULL(gabs_pbuf_ci_reserve_head(it, 3));
        GABS_ASSERT_EQ(gabs_pbuf_ci_headroom(it), 0u);
        GABS_ASSERT_EQ(gabs_pbuf_ci_size(it), sizeof(data));

        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, view_creates_readonly_subrange)
{
        static const uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
        uint8_t out[4] = {};
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 32);
        gabs_pbuf view;
        size_t copied;

        gabs_pbuf_put(&buf, data, sizeof(data));
        view = gabs_pbuf_view(buf, 2, sizeof(out), gabs_test_pbuf_alloc());

        GABS_ASSERT_TRUE(gabs_pbuf_okay(view));
        GABS_ASSERT_EQ(gabs_pbuf_size(view), sizeof(out));

        copied = gabs_pbuf_copy(view, out, 0, sizeof(out));
        GABS_ASSERT_EQ(copied, sizeof(out));
        GABS_ASSERT_EQ(memcmp(data + 2, out, sizeof(out)), 0);

        gabs_pbuf_decref(view);
        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, clone_deep_copies_subrange)
{
        static const uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7};
        uint8_t out[4] = {};
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 32);
        gabs_pbuf clone;
        size_t copied;

        gabs_pbuf_put(&buf, data, sizeof(data));
        clone = gabs_pbuf_clone(buf, 2, sizeof(out), gabs_test_pbuf_alloc());

        GABS_ASSERT_TRUE(gabs_pbuf_okay(clone));
        GABS_ASSERT_EQ(gabs_pbuf_size(clone), sizeof(out));

        copied = gabs_pbuf_copy(clone, out, 0, sizeof(out));
        GABS_ASSERT_EQ(copied, sizeof(out));
        GABS_ASSERT_EQ(memcmp(data + 2, out, sizeof(out)), 0);

        gabs_pbuf_decref(clone);
        gabs_pbuf_decref(buf);
}

GABS_TEST(pbuf, ci_cap_is_at_least_requested_size)
{
        gabs_pbuf buf = gabs_pbuf_new(gabs_test_pbuf_alloc(), 24);
        gabs_pbuf_ci it = gabs_pbuf_ci_init(&buf);

        GABS_ASSERT_TRUE(gabs_pbuf_ci_cap(it) >= 24u);

        gabs_pbuf_decref(buf);
}
