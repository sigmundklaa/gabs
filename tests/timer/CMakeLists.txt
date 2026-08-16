
add_executable(gabs-tests-timer)
target_sources(gabs-tests-timer PRIVATE timer.cc)

set_target_properties(
    gabs-tests-timer
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(gabs-tests-timer PRIVATE gabs)

catch_discover_tests(gabs-tests-timer)
