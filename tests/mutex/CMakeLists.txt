
gabs_add_test(mutex mutex.cc)
target_link_libraries(gabs-tests-mutex PRIVATE gabs-mutex-posix)
