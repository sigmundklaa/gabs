
#ifndef GABS_TIMER_MANUAL_DEF_H__
#define GABS_TIMER_MANUAL_DEF_H__

#ifndef GABS_TIMER_INCLUDE_DEF
#error "gabs_timer_def.h should not be included directly"
#endif /* GABS_TIMER_INCLUDE_DEF */

#ifndef GABS_CONFIG_TIMER_MANUAL_MAX
#define GABS_CONFIG_TIMER_MANUAL_MAX (64)
#endif

#include <stdbool.h>
#include <stdint.h>

#include <gabs/core/util.h>

GABS_BEGIN_DECL

struct gabs_timer_manual {
        void (*cb)(struct gabs_timer_manual *, void *);
        void *user_arg;

        bool used;
        bool active;
        uint64_t expiry_us;
        uint64_t seq;

        struct gabs_timer_manual_ctx *ctx;
};

struct gabs_timer_manual_ctx {
        uint64_t now_us;
        uint64_t next_seq;
        struct gabs_timer_manual timers[GABS_CONFIG_TIMER_MANUAL_MAX];
};

typedef struct gabs_timer_manual *gabs_timer;
typedef struct gabs_timer_manual_ctx gabs_timer_ctx;

GABS_END_DECL

#endif /* GABS_TIMER_MANUAL_DEF_H__ */
