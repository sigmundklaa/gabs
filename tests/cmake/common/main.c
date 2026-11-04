#include <stdio.h>

const char *timer_header(void);

/* Defined by the sources of a timer, and of the manual timer. */
__attribute__((weak)) void gabs_timer_install(void);
__attribute__((weak)) void gabs_timer_manual_now(void);

int main(void)
{
        const char *sources;

        if (gabs_timer_manual_now != NULL) {
                sources = "manual";
        } else if (gabs_timer_install != NULL) {
                sources = "posix";
        } else {
                sources = "none";
        }

        printf("%s sources=%s\n", timer_header(), sources);

        return 0;
}
