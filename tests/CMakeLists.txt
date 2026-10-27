
find_package(Catch2 3 REQUIRED)
include(Catch)

gabs_select(gabs-testing-catch2)

function(gabs_add_test name)
    add_executable(gabs-tests-${name})
    target_sources(gabs-tests-${name} PRIVATE ${ARGN})
    set_target_properties(
        gabs-tests-${name}
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
    target_link_libraries(gabs-tests-${name} PRIVATE gabs)
    catch_discover_tests(gabs-tests-${name})
endfunction()

add_subdirectory(testing)
