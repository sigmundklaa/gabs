
# No DEFAULT: a test build must select its framework explicitly. Nothing
# requires this module, so it stays out of ordinary library builds.
gabs_module(
    gabs-testing
    ADD_TO_INTERFACE
)

add_subdirectory(catch2)
add_subdirectory(ztest)
