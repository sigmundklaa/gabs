
gabs_implement(
    gabs-testing-catch2
    IMPLEMENTS
        gabs-testing
    INCLUDE
        .
)

gabs_get_lib(lib gabs-testing-catch2)
target_link_libraries(${lib} INTERFACE Catch2::Catch2WithMain)
