
cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gabs_tests_mutex)

target_sources(app PRIVATE ../mutex.cc)
