
#ifndef GABS_TIMER_POSIX_DEF_H__
#define GABS_TIMER_POSIX_DEF_H__

#ifndef GABS_TIMER_INCLUDE_DEF
#error "gabs_timer_def.h should not be included directly"
#endif /* GABS_TIMER_INCLUDE_DEF */

#ifndef GABS_CONFIG_TIMER_POSIX_MAX
#define GABS_CONFIG_TIMER_POSIX_MAX (10)
#endif

#include <pthread.h>

#ifdef __cplusplus
#include <atomic>

using atomic_uint = std::atomic_uint;
using atomic_bool = std::atomic_bool;
#else
#include <stdatomic.h>
#endif

#include <gabs/core/util.h>

GABS_BEGIN_DECL

struct gabs_timer_posix {
        void (*cb)(struct gabs_timer_posix *, void *);
        void *user_arg;

        int fd;
        atomic_uint state;

        struct gabs_timer_posix_ctx *ctx;
};

struct gabs_timer_posix_ctx {
        pthread_t thread;
        int eventfd;
        atomic_uint work_state;
        atomic_bool stopped;
        struct gabs_timer_posix timers[GABS_CONFIG_TIMER_POSIX_MAX];
};

typedef struct gabs_timer_posix *gabs_timer;
typedef struct gabs_timer_posix_ctx gabs_timer_ctx;

GABS_END_DECL

#endif /* GABS_TIMER_POSIX_DEF_H__ */
