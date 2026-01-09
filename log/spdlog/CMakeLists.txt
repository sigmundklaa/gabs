
gabs_implement(gabs-log-spdlog IMPLEMENTS gabs-log INCLUDE . SOURCES spdlog.cc)
gabs_get_lib(lib gabs-log-spdlog)

target_link_libraries(${lib} INTERFACE spdlog::spdlog)
