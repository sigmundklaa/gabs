
#include <zephyr/random/random.h>

#include <gabs/random.h>

uint32_t gabs_random(void)
{
        return sys_rand32_get() % ((1UL << 31) - 1);
}
