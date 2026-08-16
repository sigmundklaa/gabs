
add_executable(gabs-tests-semaphore)
target_sources(gabs-tests-semaphore PRIVATE semaphore.cc)

set_target_properties(
    gabs-tests-semaphore
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(gabs-tests-semaphore PRIVATE gabs)

catch_discover_tests(gabs-tests-semaphore)
