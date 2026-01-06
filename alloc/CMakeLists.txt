
gabs_module(
    gabs-alloc
    ADD_TO_INTERFACE
    ALLOW_MANY
)

gabs_implement(gabs-alloc-std IMPLEMENTS gabs-alloc SOURCES std.c)
gabs_implement(gabs-alloc-dynamic IMPLEMENTS gabs-alloc SOURCES dynamic.c)

gabs_get_iface(dyn_iface gabs-alloc-dynamic)
target_compile_definitions(${dyn_iface} INTERFACE GABS_CONFIG_DYN_ALLOC=1)
