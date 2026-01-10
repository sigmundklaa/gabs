
zephyr_interface_library_named(gabs-contrib-zephyr)

target_include_directories(gabs-contrib-zephyr INTERFACE .)
target_link_libraries(gabs-contrib-zephyr INTERFACE gabs_iface)
