
gabs_module(
    gabs-pbuf
    DEFAULT
        gabs-pbuf-basic
    REQUIRES
        gabs-alloc
)

add_subdirectory(basic)
add_subdirectory(zephyr)
