
gabs_implement(gabs-semaphore-posix IMPLEMENTS gabs-semaphore)

target_include_directories(gabs-semaphore-posix INTERFACE .)
target_sources(gabs-semaphore-posix INTERFACE semaphore_posix.c)
target_link_libraries(gabs-semaphore-posix INTERFACE gabs-contrib-posix)
