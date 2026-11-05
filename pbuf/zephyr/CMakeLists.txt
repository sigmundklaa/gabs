
gabs_implement(
    gabs-pbuf-zephyr
    IMPLEMENTS
        gabs-pbuf
    INCLUDE
        .
    SOURCES
        net_buf.c
)

gabs_get_lib(lib gabs-pbuf-zephyr)
target_link_libraries(${lib} INTERFACE gabs-alloc-zephyr-netbuf)
