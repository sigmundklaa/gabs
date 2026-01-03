
zephyr_library_named(gabs)

zephyr_interface_library_named(gabs_iface INTERFACE)
zephyr_library_link_libraries(gabs_iface)

include(${ZEPHYR_CURRENT_MODULE_DIR}/cmake/gabs.cmake)

# Add out of tree subdirectory
macro(add_oot_subdirectory name)
    add_subdirectory("${ZEPHYR_CURRENT_MODULE_DIR}/${name}" ${name})
endmacro()

add_oot_subdirectory(compiler)
add_oot_subdirectory(alloc)
add_oot_subdirectory(pbuf)
add_oot_subdirectory(log)
add_oot_subdirectory(mutex)
add_oot_subdirectory(semaphore)

gabs_provide(gabs-compiler-gcc)
gabs_provide(gabs-alloc-dynamic)
gabs_provide(gabs-log-zephyr)
gabs_provide(gabs-mutex-zephyr)
gabs_provide(gabs-semaphore-zephyr)
