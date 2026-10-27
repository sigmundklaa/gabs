
find_package(spdlog REQUIRED GLOBAL)

gabs_add_test(log log.cc capture_spdlog.cc spdlog.cc)
target_link_libraries(gabs-tests-log PRIVATE gabs-log-spdlog)
