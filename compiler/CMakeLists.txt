
add_subdirectory(gcc)

# gcc is the only supported compiler.
target_link_libraries(gabs-core INTERFACE gabs-compiler-gcc)
