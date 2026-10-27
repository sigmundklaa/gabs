
cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gabs_tests_timer_manual)

gabs_require(gabs-testing)

gabs_select(gabs-timer-manual)

target_sources(app PRIVATE ../timer_manual.cc)
