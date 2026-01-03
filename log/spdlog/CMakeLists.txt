
gabs_implement(gabs-log-spdlog IMPLEMENTS gabs-log)

target_include_directories(gabs-log-spdlog INTERFACE .)
target_sources(gabs-log-spdlog INTERFACE spdlog.cc)
