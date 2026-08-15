
find_package(Catch2 3 REQUIRED)
include(Catch)

gabs_select(gabs-testing-catch2)
gabs_require(gabs-mutex gabs-semaphore)

include(${CMAKE_CURRENT_LIST_DIR}/api/sources.cmake)

add_executable(gabs_api_tests ${GABS_TEST_API_SOURCES})

set_target_properties(
    gabs_api_tests
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(gabs_api_tests PRIVATE gabs)

catch_discover_tests(gabs_api_tests)
