
gabs_module(
    gabs-alloc
    ADD_TO_INTERFACE
    ALLOW_MANY
)

gabs_sources(gabs-alloc alloc.c)

add_subdirectory(zephyr)

gabs_implement(gabs-alloc-std IMPLEMENTS gabs-alloc SOURCES std.c)
