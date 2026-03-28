
#ifndef GABS_TIMER_ZEPHYR_DEF_H__
#define GABS_TIMER_ZEPHYR_DEF_H__

#ifndef GABS_TIMER_INCLUDE_DEF
#error "gabs_timer_def.h should not be included directly"
#endif /* GABS_TIMER_INCLUDE_DEF */

#include <zephyr/sys/mutex.h>
#include <zephyr/sys/dlist.h>

struct gabs_timer_zephyr_ctx {
        sys_dlist_t list;
        struct k_mutex lock;
};

typedef void *gabs_timer;
typedef struct gabs_timer_zephyr_ctx gabs_timer_ctx;

#endif /* GABS_TIMER_ZEPHYR_DEF_H__ */
