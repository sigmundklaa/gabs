
#include <gabs/semaphore.h>

#include <zephyr_timediff.h>

int gabs_sem_down(gabs_sem *sem, uint64_t timeout_us)
{
        return sys_sem_take(sem, zephyr_timediff_resolve(timeout_us));
}
