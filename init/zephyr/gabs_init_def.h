
#ifndef GABS_INIT_ZEPHYR_DEF_H__
#define GABS_INIT_ZEPHYR_DEF_H__

#include <zephyr/init.h>

#define GABS_INIT_2(func_, prio_)                                              \
        statc int gabs_init_##__LINE__##__(void)                               \
        {                                                                      \
                func_();                                                       \
                return 0;                                                      \
        }                                                                      \
        SYS_INIT(gabs_init_##__LINE__##__, POST_KERNEL, prio_)

#define GABS_INIT_1(func_) GABS_INIT_2(func_, 99)

#endif /* GABS_INIT_ZEPHYR_DEF_H__ */
