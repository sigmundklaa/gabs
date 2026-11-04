cmake_minimum_required(VERSION 3.20)
project(gabs_tests_cmake_link C)

include(../common/common.cmake)

add_library(source INTERFACE)
target_sources(source INTERFACE ${TIMER_HEADER})
target_link_libraries(source INTERFACE gabs-timer)

check_timer(
    source_posix
    "posix sources=posix"
    source
    gabs-timer-posix
    gabs-log-disabled
    gabs-mutex-posix
)
check_timer(source_manual "manual sources=manual" source gabs-timer-manual)
check_timer(iface "manual sources=none" source gabs-timer-manual-iface)

add_executable(module_sources ../common/alloc.c)
target_link_libraries(module_sources PRIVATE gabs-alloc-std)
add_test(NAME module_sources COMMAND module_sources)
set_tests_properties(
    module_sources
    PROPERTIES
        PASS_REGULAR_EXPRESSION "^module=yes\n$"
)
