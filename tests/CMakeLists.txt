
find_package(Catch2 3 REQUIRED)
include(Catch)

gabs_select(gabs-testing-catch2)
gabs_require(gabs-mutex gabs-semaphore)

add_subdirectory(mutex)
add_subdirectory(semaphore)
