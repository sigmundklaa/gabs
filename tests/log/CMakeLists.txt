
find_package(spdlog REQUIRED GLOBAL)

gabs_select(gabs-log-spdlog)

add_executable(gabs-tests-log)
target_sources(gabs-tests-log PRIVATE log.cc capture_spdlog.cc)

set_target_properties(
    gabs-tests-log
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(gabs-tests-log PRIVATE gabs spdlog::spdlog)

catch_discover_tests(gabs-tests-log)
