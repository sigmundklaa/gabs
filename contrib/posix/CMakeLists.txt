
add_library(gabs-contrib-posix)

target_sources(
    gabs-contrib-posix
    PRIVATE
        posix_timediff.c
)

target_include_directories(gabs-contrib-posix PUBLIC .)
target_link_libraries(gabs-contrib-posix PUBLIC gabs-core)
