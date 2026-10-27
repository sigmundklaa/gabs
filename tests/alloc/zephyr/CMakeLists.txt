
cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gabs_tests_alloc)

target_sources(app PRIVATE ../alloc.cc alloc_heap.cc alloc_zephyr.cc)
