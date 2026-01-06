
gabs_implement(
    gabs-semaphore-posix
    IMPLEMENTS
        gabs-semaphore
    INCLUDE
        .
    SOURCES
        semaphore_posix.c
)

gabs_get_lib(_lib gabs-semaphore-posix)
target_link_libraries(${_lib} INTERFACE gabs-contrib-posix)
