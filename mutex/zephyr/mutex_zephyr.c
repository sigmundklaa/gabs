
#include <gabs/mutex.h>

int gabs_mutex_lock(gabs_mutex *m, uint64_t timeout_us)
{
        int status;

        status = sys_mutex_lock(m, K_USEC(timeout_us));
        if (status == -EAGAIN) {
                return -ETIMEDOUT;
        }

        return status;
}
