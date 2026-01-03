cmake_minimum_required(VERSION 3.20)
project(gabs)

add_library(gabs)

add_library(gabs_iface INTERFACE)
target_include_directories(gabs_iface INTERFACE include)

target_link_libraries(gabs PUBLIC gabs_iface)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gabs.cmake)

add_subdirectory(contrib)
add_subdirectory(compiler)
add_subdirectory(alloc)
add_subdirectory(pbuf)
add_subdirectory(log)
add_subdirectory(mutex)
add_subdirectory(semaphore)
