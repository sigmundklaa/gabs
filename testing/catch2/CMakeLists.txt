
# QUIET: the implementation is only ever linked by a build that selects it,
# so an ordinary library build must not require Catch2 to be installed.
find_package(Catch2 3 QUIET)

gabs_implement(
    gabs-testing-catch2
    IMPLEMENTS
        gabs-testing
    INCLUDE
        .
)

gabs_get_lib(lib gabs-testing-catch2)
target_link_libraries(${lib} INTERFACE Catch2::Catch2WithMain)
