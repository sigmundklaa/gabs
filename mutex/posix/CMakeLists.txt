
gabs_implement(gabs-mutex-posix IMPLEMENTS gabs-mutex)

target_include_directories(gabs-mutex-posix INTERFACE .)
target_sources(gabs-mutex-posix INTERFACE mutex_posix.c)
target_link_libraries(gabs-mutex-posix INTERFACE gabs-contrib-posix)
