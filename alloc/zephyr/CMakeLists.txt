
gabs_implement(
    gabs-alloc-zephyr-netbuf
    IMPLEMENTS
        gabs-alloc
    SOURCES
        netbuf_alloc.c
)

gabs_implement(
    gabs-alloc-zephyr-heap
    IMPLEMENTS
        gabs-alloc
    SOURCES
        zephyr_heap.c
)
