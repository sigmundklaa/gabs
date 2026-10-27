
cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gabs_tests_log)

target_sources(app PRIVATE ../log.cc capture_uart.cc log_zephyr.cc)
