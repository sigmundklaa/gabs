
gabs_implement(gabs-pbuf-basic IMPLEMENTS gabs-pbuf)

target_sources(gabs-pbuf-basic INTERFACE basic.c)
target_include_directories(gabs-pbuf-basic INTERFACE .)
