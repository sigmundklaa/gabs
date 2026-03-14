
#ifndef GABS_POSIX_INIT_H__
#define GABS_POSIX_INIT_H__

#include <gabs/core/util.h>

GABS_BEGIN_DECL

#define POSIX_INIT_2(func_, prio_)                                             \
        static void __attribute__((constructor(100 + prio_)))                  \
        gabs_init_##__LINE__##__(void)                                         \
        {                                                                      \
                func_();                                                       \
        }

#define POSIX_INIT_1(func_) POSIX_INIT_2(func_, 99)

#define POSIX_INIT__(count_, ...)                                              \
        GABS_CONCAT_2(POSIX_INIT_, count_)(__VA_ARGS__)

/**
 * @brief Regster function to be invoked at applciation start.
 *
 * This is before after entering main(). Functions registered with a priority
 * number lower is executed first. Priority can be 0-99. If not specified,
 * the default priority is 99.
 *
 * @param func_ Function to be invoked
 * @param priority_ (Optional) priority. Default: 99
 */
#define POSIX_INIT(...)                                                        \
        POSIX_INIT__(GABS_COUNT_VA_ARGS(__VA_ARGS__), __VA_ARGS__)

#define POSIX_DEINIT_2(func_, prio_)                                           \
        static void __attribute__((destructor(100 + prio_)))                   \
        gabs_deinit_##__LINE__##__(void)                                       \
        {                                                                      \
                func_();                                                       \
        }

#define POSIX_DEINIT_1(func_) POSIX_DEINIT_2(func_, 99)

#define POSIX_DEINIT__(count_, ...)                                            \
        GABS_CONCAT_2(POSIX_DEINIT_, count_)(__VA_ARGS__)

/**
 * @brief Regster function to be invoked at applciation exit.
 *
 * This is called after main() exits. Functions registered with a priority
 * number higher is executed first. Priority can be 0-99. If not specified,
 * the default priority is 99.
 *
 * @param func_ Function to be invoked
 * @param priority_ (Optional) priority. Default: 99
 */
#define POSIX_DEINIT(...)                                                      \
        POSIX_DEINIT__(GABS_COUNT_VA_ARGS(__VA_ARGS__), __VA_ARGS__)

GABS_END_DECL

#endif /* GABS_POSIX_INIT_H__ */
