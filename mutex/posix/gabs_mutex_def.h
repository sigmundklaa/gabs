
#ifndef GABS_MUTEX_POSIX_DEF_H__
#define GABS_MUTEX_POSIX_DEF_H__

#include <pthread.h>

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

#ifndef GABS_MUTEX_INCLUDE_DEF
#error "gabs_mutex_def.h should not be included directly"
#endif /* GABS_MUTEX_INCLUDE_DEF */

typedef pthread_mutex_t gabs_mutex;

GABS_INLINE int gabs_mutex_init(gabs_mutex *lock)
{
        return -pthread_mutex_init(lock, NULL);
}

GABS_INLINE int gabs_mutex_deinit(gabs_mutex *lock)
{
        return -pthread_mutex_destroy(lock);
}

GABS_INLINE int gabs_mutex_unlock(gabs_mutex *lock)
{
        return -pthread_mutex_unlock(lock);
}

GABS_END_DECL

#endif /* GABS_MUTEX_POSIX_DEF_H__ */
