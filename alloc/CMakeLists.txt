
gabs_module(gabs-alloc)

gabs_sources(gabs-alloc alloc.c)

add_subdirectory(zephyr)

gabs_implement(gabs-alloc-std IMPLEMENTS gabs-alloc SOURCES std.c)
