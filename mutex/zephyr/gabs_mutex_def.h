
#ifndef GABS_MUTEX_ZEPHYR_DEF_H__
#define GABS_MUTEX_ZEPHYR_DEF_H__

#include <zephyr/sys/mutex.h>

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

typedef struct sys_mutex gabs_mutex;

GABS_INLINE int gabs_mutex_init(gabs_mutex *m)
{
        sys_mutex_init(m);
        return 0;
}

GABS_INLINE int gabs_mutex_deinit(gabs_mutex *m)
{
        ARG_UNUSED(m);
        return 0;
}

GABS_INLINE int gabs_mutex_unlock(gabs_mutex *m)
{
        return sys_mutex_unlock(m);
}

GABS_END_DECL

#endif /* GABS_MUTEX_ZEPHYR_DEF_H__ */
