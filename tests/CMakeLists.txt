
find_package(Catch2 3 REQUIRED)
include(Catch)

gabs_select(gabs-testing-catch2)

add_subdirectory(mutex)
add_subdirectory(semaphore)
add_subdirectory(pbuf)
add_subdirectory(timer)
