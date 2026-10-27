
find_package(Catch2 3 REQUIRED)
include(Catch)

function(gabs_add_test name)
    add_executable(gabs-tests-${name})
    target_sources(gabs-tests-${name} PRIVATE ${ARGN})
    set_target_properties(
        gabs-tests-${name}
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
    target_link_libraries(gabs-tests-${name} PRIVATE gabs-testing-catch2)
    catch_discover_tests(gabs-tests-${name})
endfunction()

add_subdirectory(testing)
add_subdirectory(timer)
add_subdirectory(mutex)
add_subdirectory(semaphore)
add_subdirectory(pbuf)
add_subdirectory(log)
add_subdirectory(alloc)
add_subdirectory(cmake)
