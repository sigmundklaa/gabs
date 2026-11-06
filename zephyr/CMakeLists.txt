
add_library(gabs-core INTERFACE)
target_include_directories(
    gabs-core
    INTERFACE
        ${ZEPHYR_CURRENT_MODULE_DIR}/include
)

include(${ZEPHYR_CURRENT_MODULE_DIR}/cmake/gabs.cmake)

# Add out of tree subdirectory
macro(add_oot_subdirectory name)
    add_subdirectory("${ZEPHYR_CURRENT_MODULE_DIR}/${name}" ${name})
endmacro()

add_oot_subdirectory(toolchain)
add_oot_subdirectory(alloc)
add_oot_subdirectory(pbuf)
add_oot_subdirectory(log)
add_oot_subdirectory(mutex)
add_oot_subdirectory(semaphore)
add_oot_subdirectory(random)
add_oot_subdirectory(contrib/zephyr)
add_oot_subdirectory(timer)
add_oot_subdirectory(testing)

# Headers of the modules and implementations enabled in Kconfig, for Zephyr
# libraries other than `app`.
add_library(gabs-iface INTERFACE)
target_link_libraries(gabs-iface INTERFACE gabs-core)

# Compile `lib` in `app`, and link its headers to `gabs-iface`, if `option`
# is enabled. Zephyr links every library whole, so `lib` is compiled only in
# `app`.
function(gabs_zephyr_link option lib)
    if(${option})
        target_link_libraries(app PRIVATE ${lib})
        target_link_libraries(gabs-iface INTERFACE ${lib}-iface)
    endif()
endfunction()

gabs_zephyr_link(CONFIG_GABS_ALLOC gabs-alloc)
gabs_zephyr_link(CONFIG_GABS_ALLOC_STD gabs-alloc-std)
gabs_zephyr_link(CONFIG_GABS_ALLOC_ZEPHYR_HEAP gabs-alloc-zephyr-heap)
gabs_zephyr_link(CONFIG_GABS_ALLOC_ZEPHYR_NETBUF gabs-alloc-zephyr-netbuf)
gabs_zephyr_link(CONFIG_GABS_LOG gabs-log-zephyr)
gabs_zephyr_link(CONFIG_GABS_MUTEX gabs-mutex-zephyr)
gabs_zephyr_link(CONFIG_GABS_PBUF gabs-pbuf-zephyr)
gabs_zephyr_link(CONFIG_GABS_RANDOM gabs-random-zephyr)
gabs_zephyr_link(CONFIG_GABS_SEMAPHORE gabs-semaphore-zephyr)
gabs_zephyr_link(CONFIG_GABS_TIMER_ZEPHYR gabs-timer-zephyr)
gabs_zephyr_link(CONFIG_GABS_TIMER_MANUAL gabs-timer-manual)
gabs_zephyr_link(CONFIG_GABS_TESTING gabs-testing-ztest)
