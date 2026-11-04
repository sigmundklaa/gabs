
cmake_minimum_required(VERSION 3.20)

# A library from another module, which uses the timer.
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/module)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gabs_tests_cmake)

target_sources(app PRIVATE zephyr.cc)
