
gabs_implement(gabs-compiler-gcc IMPLEMENTS gabs-compiler)

target_include_directories(
    gabs-compiler-gcc
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

