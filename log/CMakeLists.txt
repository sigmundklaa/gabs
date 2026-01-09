
gabs_module(
    gabs-log
    ADD_TO_INTERFACE
    DEFAULT
        gabs-log-disabled
)

add_subdirectory(disabled)
add_subdirectory(spdlog)
add_subdirectory(zephyr)
