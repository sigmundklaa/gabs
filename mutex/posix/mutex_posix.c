
#include <gabs/mutex.h>

#include <posix_timediff.h>

int gabs_mutex_lock(gabs_mutex *lock, uint64_t timeout)
{
        struct timespec spec;
        int status;

        if (timeout == 0) {
                return -pthread_mutex_trylock(lock);
        } else if (timeout == UINT64_MAX) {
                return -pthread_mutex_lock(lock);
        }

        status = posix_timediff_resolve(timeout, &spec);
        if (status != 0) {
                return status;
        }

        return -pthread_mutex_timedlock(lock, &spec);
}
