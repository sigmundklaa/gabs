
gabs_implement(
    gabs-testing-catch2
    IMPLEMENTS
        gabs-testing
    INCLUDE
        .
)

gabs_get_iface(iface gabs-testing-catch2)
target_link_libraries(${iface} INTERFACE Catch2::Catch2WithMain)
