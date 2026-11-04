
add_test(
    NAME cmake::link
    COMMAND
        ${CMAKE_CTEST_COMMAND}
        --build-and-test
            ${CMAKE_CURRENT_SOURCE_DIR}/link
            ${CMAKE_CURRENT_BINARY_DIR}/link
        --build-generator ${CMAKE_GENERATOR}
        --build-options -DGABS_SOURCE_DIR=${PROJECT_SOURCE_DIR}
        --test-command ${CMAKE_CTEST_COMMAND} --output-on-failure
)
