
#include <cerrno>

#include <zephyr/net_buf.h>
#include <zephyr/ztest.h>

#include <gabs/alloc/zephyr/heap.h>
#include <gabs/alloc/zephyr/netbuf.h>

K_HEAP_DEFINE(small_heap, 256);
GABS_ALLOC_ZEPHYR_HEAP_DEFINE(small_allocator, &small_heap);

NET_BUF_POOL_VAR_DEFINE(test_pool, 2, 256, 0, nullptr);
GABS_ALLOC_ZEPHYR_NETBUF_DEFINE(netbuf_allocator, &test_pool);

ZTEST(alloc_zephyr, heap_allocator_is_heap_api)
{
        zassert_true(gabs_allocator_is(small_allocator,
                                       gabs_zephyr_heap_alloc_api()));
        zassert_false(gabs_allocator_is(small_allocator,
                                        gabs_zephyr_netbuf_alloc_api()));
}

ZTEST(alloc_zephyr, heap_alloc_fails_when_heap_is_full)
{
        void *mem = nullptr;

        zassert_equal(gabs_alloc(small_allocator, 1024, &mem), -ENOMEM);
}

ZTEST(alloc_zephyr, netbuf_allocator_is_netbuf_api)
{
        zassert_true(gabs_allocator_is(netbuf_allocator,
                                       gabs_zephyr_netbuf_alloc_api()));
}

ZTEST(alloc_zephyr, netbuf_alloc_returns_net_buf_of_size)
{
        struct net_buf *buf = nullptr;

        zassert_equal(gabs_alloc(netbuf_allocator, 100,
                                 reinterpret_cast<void **>(&buf)),
                      0);
        zassert_not_null(buf);
        zassert_true(net_buf_tailroom(buf) >= 100);
        zassert_equal(buf->len, 0);

        net_buf_unref(buf);
}

ZTEST(alloc_zephyr, netbuf_alloc_fails_when_pool_is_empty)
{
        struct net_buf *first = nullptr;
        struct net_buf *second = nullptr;
        struct net_buf *third = nullptr;
        int status;

        zassert_equal(gabs_alloc(netbuf_allocator, 8,
                                 reinterpret_cast<void **>(&first)),
                      0);
        zassert_equal(gabs_alloc(netbuf_allocator, 8,
                                 reinterpret_cast<void **>(&second)),
                      0);
        status = gabs_alloc(netbuf_allocator, 8,
                            reinterpret_cast<void **>(&third));

        net_buf_unref(first);
        net_buf_unref(second);

        zassert_equal(status, -ENOMEM);
}

ZTEST_SUITE(alloc_zephyr, nullptr, nullptr, nullptr, nullptr, nullptr);
