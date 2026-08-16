
add_executable(gabs-tests-mutex)
target_sources(gabs-tests-mutex PRIVATE mutex.cc)

set_target_properties(
    gabs-tests-mutex
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(gabs-tests-mutex PRIVATE gabs)

catch_discover_tests(gabs-tests-mutex)
