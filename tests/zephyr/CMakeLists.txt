
cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gabs_api_tests)

gabs_select(gabs-testing-ztest)
gabs_require(gabs-mutex gabs-semaphore)

include(${CMAKE_CURRENT_LIST_DIR}/../api/sources.cmake)

target_sources(app PRIVATE ${GABS_TEST_API_SOURCES})
