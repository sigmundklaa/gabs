
gabs_implement(
    gabs-semaphore-zephyr
    IMPLEMENTS
        gabs-semaphore
    INCLUDE
        .
    SOURCES
        semaphore_zephyr.c
)

gabs_get_lib(lib gabs-semaphore-zephyr)
target_link_libraries(${lib} INTERFACE gabs-contrib-zephyr)
