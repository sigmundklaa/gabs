
gabs_require(gabs-pbuf)

# pbuf requires an allocator; provide the dynamic dispatcher plus a concrete
# (std/malloc) backend for it to dispatch to.
gabs_provide(gabs-alloc-dynamic gabs-alloc-std)
gabs_require(gabs-alloc)

add_executable(gabs-tests-pbuf)
target_sources(gabs-tests-pbuf PRIVATE pbuf.cc alloc_std.c)

set_target_properties(
    gabs-tests-pbuf
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(gabs-tests-pbuf PRIVATE gabs)

catch_discover_tests(gabs-tests-pbuf)
