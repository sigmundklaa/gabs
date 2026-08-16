
cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gabs_tests_mutex)

gabs_require(gabs-testing gabs-mutex)

target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../mutex.cc)
