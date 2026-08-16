
cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gabs_tests_semaphore)

gabs_select(gabs-testing-ztest)
gabs_require(gabs-semaphore)

target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../semaphore.cc)
