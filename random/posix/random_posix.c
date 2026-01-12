

#include <stdlib.h>
#include <time.h>

#include <gabs/random.h>
#include <gabs/init.h>

uint32_t gabs_random(void)
{
        return random();
}

static void seed(void)
{
        srandom(time(NULL));
}

GABS_INIT(seed);
