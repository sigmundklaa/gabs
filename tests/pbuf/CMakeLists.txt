
gabs_add_test(pbuf pbuf.cc alloc_std.cc)
target_link_libraries(
    gabs-tests-pbuf
    PRIVATE
        gabs-pbuf-basic
        gabs-alloc-std
)
