
#include <gabs/semaphore.h>

#include <posix_timediff.h>

int gabs_sem_down(gabs_sem *sem, uint64_t timeout)
{
        struct timespec spec;
        int status;

        if (timeout == 0) {
                return -sem_trywait(sem);
        } else if (timeout == UINT64_MAX) {
                return -sem_wait(sem);
        }

        status = posix_timediff_resolve(timeout, &spec);
        if (status != 0) {
                return status;
        }

        return -sem_timedwait(sem, &spec);
}
