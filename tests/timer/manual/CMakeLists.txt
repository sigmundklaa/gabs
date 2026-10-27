
# The host tests share one gabs library, built with another timer backend.
gabs_add_test(
    timer-manual
    timer_manual.cc
    ${PROJECT_SOURCE_DIR}/timer/manual/timer_manual.c
)
target_include_directories(
    gabs-tests-timer-manual
    BEFORE
    PRIVATE
        ${PROJECT_SOURCE_DIR}/timer/manual
)
