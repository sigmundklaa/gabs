
add_subdirectory(manual)

gabs_add_test(timer timer.cc)
target_link_libraries(
    gabs-tests-timer
    PRIVATE
        gabs-timer-posix
        gabs-log-disabled
        gabs-mutex-posix
        gabs-semaphore-posix
)
