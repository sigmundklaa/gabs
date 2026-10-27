
gabs_add_test(alloc alloc.cc alloc_std.cc std.cc)
target_link_libraries(gabs-tests-alloc PRIVATE gabs-alloc-std)
