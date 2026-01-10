
gabs_module(
    gabs-pbuf
    ADD_TO_INTERFACE
    DEFAULT
        gabs-pbuf-basic
    REQUIRES
        gabs-alloc
)
gabs_sources(gabs-pbuf buf.c)

add_subdirectory(basic)
add_subdirectory(zephyr)
