
add_subdirectory(gcc)

# gcc is the only supported toolchain.
target_link_libraries(gabs-core INTERFACE gabs-toolchain-gcc)
