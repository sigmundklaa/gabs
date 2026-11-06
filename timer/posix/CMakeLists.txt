
gabs_implement(
    gabs-timer-posix
    IMPLEMENTS
        gabs-timer
    REQUIRES
        gabs-log
        gabs-mutex
    INCLUDE
        .
    SOURCES
        timer_posix.c
)
