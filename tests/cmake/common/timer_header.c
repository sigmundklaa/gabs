#include <gabs/timer.h>

const char *timer_header(void)
{
#if defined(GABS_TIMER_POSIX_DEF_H__)
        return "posix";
#elif defined(GABS_TIMER_MANUAL_DEF_H__)
        return "manual";
#else
        return "unknown";
#endif
}
