

#include <stdlib.h>
#include <time.h>

#include <gabs/random.h>

#include <posix_init.h>

uint32_t gabs_random(void)
{
        return random();
}

POSIX_INIT(10)
{
        srandom(time(NULL));
}
