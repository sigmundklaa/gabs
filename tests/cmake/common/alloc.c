#include <stdio.h>

/* Defined by the sources of the gabs-alloc module. */
__attribute__((weak)) void gabs_alloc(void);

int main(void)
{
        printf("module=%s\n", gabs_alloc != NULL ? "yes" : "no");

        return 0;
}
