
gabs_implement(gabs-pbuf-zephyr IMPLEMENTS gabs-pbuf)

target_sources(gabs-pbuf-zephyr INTERFACE net_buf.c)
target_include_directories(gabs-pbuf-zephyr INTERFACE .)
