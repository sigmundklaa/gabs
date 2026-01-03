
gabs_module(
    gabs-alloc
    ADD_ALWAYS
    ADD_TO_INTERFACE
    DEFAULT
        gabs-alloc-dynamic
)

gabs_implement(gabs-alloc-dynamic IMPLEMENTS gabs-alloc)

target_sources(gabs-alloc-dynamic INTERFACE dynamic.c)
