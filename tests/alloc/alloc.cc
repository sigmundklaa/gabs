
#include <errno.h>
#include <string.h>

#include <gabs/testing/testing.h>

#include <gabs/alloc.h>

#include "platform_alloc.hh"

namespace
{

struct fake_allocator {
        gabs_allocator allocator;
        const gabs_allocator_h *alloc_handle;
        const gabs_allocator_h *dealloc_handle;
        size_t size;
        void *mem;
        int status;
};

uint8_t fake_mem[16];

fake_allocator *fake_from(const gabs_allocator_h *handle)
{
        return gabs_container_of(GABS_ALLOCATOR_GET(handle), fake_allocator,
                                 allocator);
}

int fake_alloc(const gabs_allocator_h *handle, size_t size, void **mem)
{
        fake_allocator *fake = fake_from(handle);

        fake->alloc_handle = handle;
        fake->size = size;
        *mem = fake_mem;

        return fake->status;
}

int fake_dealloc(const gabs_allocator_h *handle, void *mem)
{
        fake_allocator *fake = fake_from(handle);

        fake->dealloc_handle = handle;
        fake->mem = mem;

        return fake->status;
}

const gabs_allocator_api fake_api = {fake_alloc, fake_dealloc};
const gabs_allocator_api other_api = {fake_alloc, fake_dealloc};

} // namespace

GABS_SUITE(alloc);

GABS_TEST(alloc, alloc_calls_api_with_handle)
{
        fake_allocator fake = {GABS_ALLOCATOR_INIT(&fake_api)};
        const gabs_allocator_h *handle = GABS_ALLOCATOR_HANDLE(&fake.allocator);
        void *mem = nullptr;

        GABS_ASSERT_EQ(gabs_alloc(handle, 12, &mem), 0);
        GABS_ASSERT_EQ(fake.alloc_handle, handle);
        GABS_ASSERT_EQ(fake.size, 12u);
        GABS_ASSERT_EQ(mem, static_cast<void *>(fake_mem));
}

GABS_TEST(alloc, dealloc_calls_api_with_handle)
{
        fake_allocator fake = {GABS_ALLOCATOR_INIT(&fake_api)};
        const gabs_allocator_h *handle = GABS_ALLOCATOR_HANDLE(&fake.allocator);

        GABS_ASSERT_EQ(gabs_dealloc(handle, fake_mem), 0);
        GABS_ASSERT_EQ(fake.dealloc_handle, handle);
        GABS_ASSERT_EQ(fake.mem, static_cast<void *>(fake_mem));
}

GABS_TEST(alloc, alloc_and_dealloc_return_api_status)
{
        fake_allocator fake = {GABS_ALLOCATOR_INIT(&fake_api)};
        const gabs_allocator_h *handle = GABS_ALLOCATOR_HANDLE(&fake.allocator);
        void *mem = nullptr;

        fake.status = -ENOMEM;

        GABS_ASSERT_EQ(gabs_alloc(handle, 12, &mem), -ENOMEM);
        GABS_ASSERT_EQ(gabs_dealloc(handle, fake_mem), -ENOMEM);
}

GABS_TEST(alloc, allocator_is_its_api)
{
        fake_allocator fake = {GABS_ALLOCATOR_INIT(&fake_api)};
        const gabs_allocator_h *handle = GABS_ALLOCATOR_HANDLE(&fake.allocator);

        GABS_ASSERT_TRUE(gabs_allocator_is(handle, &fake_api));
        GABS_ASSERT_FALSE(gabs_allocator_is(handle, &other_api));
}

GABS_TEST(alloc, platform_alloc_is_writable_until_dealloc)
{
        const gabs_allocator_h *alloc = gabs::tests::platform_allocator();
        void *first = nullptr;
        void *second = nullptr;

        GABS_ASSERT_EQ(gabs_alloc(alloc, 64, &first), 0);
        GABS_ASSERT_NOT_NULL(first);
        memset(first, 0xa5, 64);

        GABS_EXPECT_EQ(gabs_alloc(alloc, 64, &second), 0);
        GABS_EXPECT_NOT_NULL(second);
        if (second != nullptr) {
                memset(second, 0x5a, 64);
                GABS_EXPECT_EQ(gabs_dealloc(alloc, second), 0);
        }

        GABS_EXPECT_EQ(static_cast<uint8_t *>(first)[63], 0xa5);
        GABS_ASSERT_EQ(gabs_dealloc(alloc, first), 0);
}
