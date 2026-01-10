
#include <gabs/mutex.h>

#include <zephyr_timediff.h>

int gabs_mutex_lock(gabs_mutex *m, uint64_t timeout_us)
{
        int status;

        status = sys_mutex_lock(m, zephyr_timediff_resolve(timeout_us));
        if (status == -EAGAIN) {
                return -ETIMEDOUT;
        }

        return status;
}
