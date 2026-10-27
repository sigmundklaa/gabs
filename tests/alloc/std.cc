
#include <string.h>

#include <catch2/catch_test_macros.hpp>

#include <gabs/alloc.h>
#include <gabs/alloc.hh>
#include <gabs/alloc/std.h>
#include <gabs/alloc/std.hh>

TEST_CASE("alloc_std::std_allocator_is_std_api", "[alloc_std]")
{
        REQUIRE(gabs_allocator_is(gabs_std_allocator(), gabs_std_alloc_api()));
}

TEST_CASE("alloc_std::handle_converts_back", "[alloc_std]")
{
        gabs::memory::allocator_handle handle(gabs_std_alloc_api());

        REQUIRE(gabs::memory::allocator_handle::from(handle.to()) == &handle);
        REQUIRE(gabs_allocator_is(handle.to(), gabs_std_alloc_api()));
}

TEST_CASE("alloc_std::cc_allocator_allocates", "[alloc_std]")
{
        gabs::memory::allocator<> allocator;
        const gabs_allocator_h *handle = allocator;
        void *mem = nullptr;

        REQUIRE(gabs_allocator_is(handle, gabs::memory::allocator<>::api()));
        REQUIRE(gabs::memory::allocator<>::from_handle(handle) == &allocator);

        REQUIRE(gabs_alloc(handle, 64, &mem) == 0);
        REQUIRE(mem != nullptr);
        memset(mem, 0xa5, 64);
        REQUIRE(gabs_dealloc(handle, mem) == 0);
}
