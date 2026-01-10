
gabs_implement(
    gabs-mutex-zephyr
    IMPLEMENTS
        gabs-mutex
    INCLUDE
        .
    SOURCES
        mutex_zephyr.c
)

gabs_get_lib(lib gabs-mutex-zephyr)
target_link_libraries(${lib} INTERFACE gabs-contrib-zephyr)
