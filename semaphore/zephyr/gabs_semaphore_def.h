
#ifndef GABS_SEMAPHORE_ZEPHYR_DEF_H__
#define GABS_SEMAPHORE_ZEPHYR_DEF_H__

#ifndef GABS_SEMAPHORE_INCLUDE_DEF
#error "gabs_semaphore_def.h should not be included directly"
#endif /* GABS_SEMAPHORE_INCLUDE_DEF */

#include <zephyr/sys/semaphore.h>

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

typedef struct sys_sem gabs_sem;

GABS_INLINE int gabs_sem_init(gabs_sem *sem, unsigned int initial)
{
        return sys_sem_init(sem, initial, K_SEM_MAX_LIMIT);
}

GABS_INLINE int gabs_sem_deinit(gabs_sem *sem)
{
        ARG_UNUSED(sem);
        return 0;
}

GABS_INLINE int gabs_sem_up(gabs_sem *sem)
{
        return sys_sem_give(sem);
}

GABS_INLINE int gabs_sem_down(gabs_sem *sem, uint64_t timeout_us)
{
        return sys_sem_take(sem, K_USEC(timeout_us));
}

GABS_END_DECL

#endif /* GABS_SEMAPHORE_ZEPHYR_DEF_H__ */
