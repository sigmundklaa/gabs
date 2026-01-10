
#ifndef GABS_CONTRIB_ZEPHYR_TIMEDIFF_H__
#define GABS_CONTRIB_ZEPHYR_TIMEDIFF_H__

#include <zephyr/kernel.h>

#include <gabs/core/api.h>
#include <gabs/common.h>

GABS_INLINE k_timeout_t zephyr_timediff_resolve(uint64_t timeout_us)
{
        if (timeout_us == 0) {
                return K_NO_WAIT;
        } else if (timeout_us == GABS_TIMEOUT_MAX) {
                return K_FOREVER;
        }

        return K_USEC(timeout_us);
}

#endif /* GABS_CONTRIB_ZEPHYR_TIMEDIFF_H__ */
