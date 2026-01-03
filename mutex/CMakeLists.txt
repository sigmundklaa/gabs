
gabs_module(
    gabs-mutex
    DEFAULT
        gabs-mutex-posix
    ADD_TO_INTERFACE
)

add_subdirectory(posix)
add_subdirectory(zephyr)
