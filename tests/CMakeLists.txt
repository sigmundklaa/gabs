
find_package(Catch2 3 REQUIRED)
include(Catch)

gabs_select(gabs-testing-catch2)
gabs_require(gabs-mutex gabs-semaphore gabs-pbuf gabs-timer gabs-log)

# pbuf requires an allocator; provide the dynamic dispatcher plus a concrete
# (std/malloc) backend for it to dispatch to.
gabs_provide(gabs-alloc-dynamic gabs-alloc-std)
gabs_require(gabs-alloc)

add_subdirectory(mutex)
add_subdirectory(semaphore)
add_subdirectory(pbuf)
add_subdirectory(timer)
