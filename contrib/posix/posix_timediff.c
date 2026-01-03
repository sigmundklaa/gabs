
#include <errno.h>

#include "posix_timediff.h"

int posix_timediff_resolve(uint64_t diff, struct timespec *spec)
{
        if (clock_gettime(CLOCK_REALTIME, spec) != 0) {
                return -errno;
        }

        spec->tv_sec += diff / (uint64_t)1e6;
        spec->tv_nsec += (diff * (uint64_t)1e3) % (uint64_t)1e9;

        if (spec->tv_nsec >= (uint64_t)1e9) {
                spec->tv_sec += 1;
                spec->tv_nsec -= (uint64_t)1e9;
        }

        return 0;
}
