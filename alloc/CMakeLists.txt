
gabs_module(
    gabs-alloc
    ADD_TO_INTERFACE
    DEFAULT
        gabs-alloc-dynamic
)

gabs_implement(gabs-alloc-dynamic IMPLEMENTS gabs-alloc SOURCES dynamic.c)
