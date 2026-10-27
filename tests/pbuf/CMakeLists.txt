
gabs_add_test(pbuf pbuf.cc alloc_std.c)
target_link_libraries(
    gabs-tests-pbuf
    PRIVATE
        gabs-pbuf-basic
        gabs-alloc-std
)
