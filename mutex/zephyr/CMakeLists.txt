
gabs_implement(gabs-mutex-zephyr IMPLEMENTS gabs-mutex)

target_include_directories(gabs-mutex-zephyr INTERFACE .)
target_sources(gabs-mutex-zephyr INTERFACE mutex_zephyr.c)
