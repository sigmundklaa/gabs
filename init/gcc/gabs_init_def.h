
#ifndef GABS_INIT_GCC_DEF_H__
#define GABS_INIT_GCC_DEF_H__

#define GABS_INIT_2(func_, prio_)                                              \
        static void __attribute__((constructor(100 + prio_)))                  \
        gabs_init_##__LINE__##__(void)                                         \
        {                                                                      \
                func_();                                                       \
        }

#define GABS_INIT_1(func_) GABS_INIT_2(func_, 99)

#endif /* GABS_INIT_GCC_DEF_H__ */
