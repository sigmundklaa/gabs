
#include <errno.h>

#include <gabs/semaphore.h>

#include <posix_timediff.h>

static int to_errno(int status)
{
        return status < 0 ? -errno : status;
}

int gabs_sem_down(gabs_sem *sem, uint64_t timeout)
{
        struct timespec spec;
        int status;

        if (timeout == 0) {
                return to_errno(sem_trywait(sem));
        } else if (timeout == UINT64_MAX) {
                return to_errno(sem_wait(sem));
        }

        status = posix_timediff_resolve(timeout, &spec);
        if (status != 0) {
                return status;
        }

        return to_errno(sem_timedwait(sem, &spec));
}
