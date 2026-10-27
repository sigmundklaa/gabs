
gabs_add_test(semaphore semaphore.cc)
target_link_libraries(gabs-tests-semaphore PRIVATE gabs-semaphore-posix)
