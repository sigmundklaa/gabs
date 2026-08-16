
#include <gabs/testing/testing.h>

#include <gabs/log.h>

/* `GABS_LOGGER_DECLARE` expands to file-scope declarations (module/instance
 * registration on the Zephyr backend), so loggers are declared here rather
 * than inside a test body. */
GABS_LOGGER_DECLARE(log_test_logger, log_tests);
GABS_LOGGER_DECLARE(log_test_other_logger, log_tests_other);

GABS_SUITE(log);

GABS_TEST(log, debug_logs_literal_message)
{
        gabs_log_dbgf(log_test_logger, "literal debug message");
}

GABS_TEST(log, info_logs_literal_message)
{
        gabs_log_inff(log_test_logger, "literal info message");
}

GABS_TEST(log, warn_logs_literal_message)
{
        gabs_log_wrnf(log_test_logger, "literal warn message");
}

GABS_TEST(log, error_logs_literal_message)
{
        gabs_log_errf(log_test_logger, "literal error message");
}

GABS_TEST(log, formats_variadic_arguments)
{
        gabs_log_errf(log_test_logger, "int=%d str=%s float=%.2f", 42,
                      "text", 3.5);
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
