#include <string.h>

#include <gabs/testing/testing.h>

#include <gabs/timer.h>
#include <gabs/timer/manual.h>

extern "C" const char *timer_header(void);

GABS_SUITE(cmake_zephyr);

GABS_TEST(cmake_zephyr, other_library_uses_kconfig_implementation)
{
        GABS_EXPECT_EQ(strcmp(timer_header(), "manual"), 0);
}

GABS_TEST(cmake_zephyr, application_uses_kconfig_implementation)
{
        gabs_timer_ctx ctx;

        GABS_ASSERT_EQ(gabs_timer_ctx_init(&ctx), 0);
        GABS_EXPECT_EQ(gabs_timer_manual_now(&ctx), 0u);
        GABS_EXPECT_EQ(gabs_timer_ctx_deinit(&ctx), 0);
}
