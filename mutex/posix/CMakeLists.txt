
gabs_implement(
    gabs-mutex-posix
    IMPLEMENTS
        gabs-mutex
    INCLUDE
        .
    SOURCES
        mutex_posix.c
)

gabs_get_lib(_lib gabs-mutex-posix)
target_link_libraries(${_lib} INTERFACE gabs-contrib-posix)
