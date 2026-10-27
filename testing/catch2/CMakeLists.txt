
find_package(Catch2 3 QUIET)
find_package(Threads QUIET)

gabs_implement(
    gabs-testing-catch2
    IMPLEMENTS
        gabs-testing
    INCLUDE
        .
    SOURCES
        main.cc
)

gabs_get_iface(iface gabs-testing-catch2)
target_link_libraries(${iface} INTERFACE Catch2::Catch2 Threads::Threads)
