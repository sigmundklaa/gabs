
gabs_implement(
    gabs-random-posix
    IMPLEMENTS
        gabs-random
    SOURCES
        random_posix.c
)

gabs_get_lib(_lib gabs-random-posix)
target_link_libraries(${_lib} INTERFACE gabs-contrib-posix)
