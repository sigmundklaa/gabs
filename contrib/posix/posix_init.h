
#ifndef GABS_POSIX_INIT_H__
#define GABS_POSIX_INIT_H__

#include <gabs/core/util.h>

GABS_BEGIN_DECL

/**
 * @brief Regster function to be invoked at applciation start.
 *
 * This is before after entering main(). Functions registered with a priority
 * number lower is executed first. Priority can be 0-99.
 *
 * Example usage:
 * ```
 * POSIX_INIT(50) {
 *         my_init_function();
 * }
 * ```
 */
#define POSIX_INIT(prio_)                                                      \
        static void __attribute__((constructor(100 + prio_)))                  \
        gabs_init_##__LINE__##__(void)

/**
 * @brief Regster function to be invoked at applciation exit.
 *
 * This is called after main() exits. Functions registered with a priority
 * number higher is executed first. Priority can be 0-99.
 *
 * Example usage:
 * ```
 * POSIX_DEINIT(50) {
 *         my_deinit_function();
 * }
 * ```
 */
#define POSIX_DEINIT(prio_)                                                    \
        static void __attribute__((destructor(100 + prio_)))                   \
        gabs_deinit_##__LINE__##__(void)

GABS_END_DECL

#endif /* GABS_POSIX_INIT_H__ */
