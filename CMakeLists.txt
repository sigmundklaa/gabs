cmake_minimum_required(VERSION 3.20)
project(gabs)

option(GABS_BUILD_TESTS "Build the gabs tests" OFF)

add_library(gabs-core INTERFACE)
target_include_directories(gabs-core INTERFACE include)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gabs.cmake)

add_subdirectory(compiler)
add_subdirectory(contrib/posix)
add_subdirectory(alloc)
add_subdirectory(pbuf)
add_subdirectory(log)
add_subdirectory(mutex)
add_subdirectory(semaphore)
add_subdirectory(random)
add_subdirectory(timer)
add_subdirectory(testing)

if(GABS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
