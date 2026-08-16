
#include <gabs/testing/testing.h>

#include <gabs/log.h>

#include "log_capture.h"

/* `GABS_LOGGER_DECLARE` expands to file-scope declarations (module/instance
 * registration on the Zephyr backend), so loggers are declared here rather
 * than inside a test body. */
GABS_LOGGER_DECLARE(log_test_logger, log_tests);
GABS_LOGGER_DECLARE(log_test_other_logger, log_tests_other);

GABS_SUITE(log);

GABS_TEST(log, debug_message_is_logged)
{
        gabs_test_log_reset();
        gabs_log_dbgf(gabs_test_log_handle(), "debug marker 1");

        GABS_ASSERT_TRUE(gabs_test_log_contains("debug marker 1"));
}

GABS_TEST(log, info_message_is_logged)
{
        gabs_test_log_reset();
        gabs_log_inff(gabs_test_log_handle(), "info marker 2");

        GABS_ASSERT_TRUE(gabs_test_log_contains("info marker 2"));
}

GABS_TEST(log, warn_message_is_logged)
{
        gabs_test_log_reset();
        gabs_log_wrnf(gabs_test_log_handle(), "warn marker 3");

        GABS_ASSERT_TRUE(gabs_test_log_contains("warn marker 3"));
}

GABS_TEST(log, error_message_is_logged)
{
        gabs_test_log_reset();
        gabs_log_errf(gabs_test_log_handle(), "error marker 4");

        GABS_ASSERT_TRUE(gabs_test_log_contains("error marker 4"));
}

GABS_TEST(log, variadic_arguments_are_formatted_into_message)
{
        gabs_test_log_reset();
        gabs_log_errf(gabs_test_log_handle(),
                      "int=%d str=%s float=%.2f", 42, "text", 3.5);

        GABS_ASSERT_TRUE(gabs_test_log_contains(
                "int=42 str=text float=3.50"));
}

GABS_TEST(log, distinctly_declared_loggers_are_independently_usable)
{
        gabs_log_inff(log_test_logger, "from log_test_logger");
        gabs_log_inff(log_test_other_logger, "from log_test_other_logger");
}

GABS_TEST(log, null_handle_does_not_crash)
{
        const gabs_logger_h *null_handle = nullptr;

        gabs_log_errf(null_handle, "message via a null handle");
}
