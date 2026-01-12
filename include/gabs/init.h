
#ifndef GABS_INIT_H__
#define GABS_INIT_H__

#include <gabs/core/util.h>

#define GABS_INIT_INCLUDE_DEF
#include <gabs_init_def.h>
#undef GABS_INIT_INCLUDE_DEF

#define GABS_INIT__(func_, count_, ...)                                        \
        GABS_CONCAT_2(GABS_INIT_, count_)(func_, ##__VA_ARGS__)

#define GABS_INIT(func_, ...)                                                  \
        GABS_INIT__(func_, GABS_COUNT_VA_ARGS(__VA_ARGS__), ##__VA_ARGS__)

#endif /* GABS_INIT_H__ */
