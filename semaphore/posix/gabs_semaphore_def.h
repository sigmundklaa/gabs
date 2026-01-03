
#ifndef GABS_SEMAPHORE_POSIX_DEF_H__
#define GABS_SEMAPHORE_POSIX_DEF_H__

#include <semaphore.h>

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

#ifndef GABS_SEMAPHORE_INCLUDE_DEF
#error "gabs_semaphore_def.h should not be included directly"
#endif /* GABS_SEMAPHORE_INCLUDE_DEF */

typedef sem_t gabs_sem;

GABS_INLINE int gabs_sem_init(gabs_sem *sem, unsigned int initial)
{
        return -sem_init(sem, 0, initial);
}

GABS_INLINE int gabs_sem_deinit(gabs_sem *sem)
{
        return -sem_destroy(sem);
}

GABS_INLINE int gabs_sem_up(gabs_sem *sem)
{
        return -sem_post(sem);
}

GABS_END_DECL

#endif /* GABS_SEMAPHORE_POSIX_DEF_H__ */
