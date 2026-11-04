
zephyr_library_named(gabs_tests_cmake)
zephyr_library_sources(${ZEPHYR_CURRENT_MODULE_DIR}/../../common/timer_header.c)
zephyr_library_link_libraries(gabs-iface)
