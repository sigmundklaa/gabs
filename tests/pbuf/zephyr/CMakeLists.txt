
cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gabs_tests_pbuf)

gabs_require(gabs-testing gabs-pbuf gabs-alloc)

target_sources(
    app
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../pbuf.cc
        alloc_netbuf.c
)
