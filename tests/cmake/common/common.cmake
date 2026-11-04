
add_subdirectory(${GABS_SOURCE_DIR} gabs)

enable_testing()

set(TIMER_HEADER ${CMAKE_CURRENT_LIST_DIR}/timer_header.c)

# Each executable prints the timer whose headers it was compiled with, and
# whose sources it links.
function(check_timer name expected)
    add_executable(${name} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/main.c)
    target_link_libraries(${name} PRIVATE ${ARGN})
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(
        ${name}
        PROPERTIES
            PASS_REGULAR_EXPRESSION "^${expected}\n$"
    )
endfunction()
