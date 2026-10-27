
#include <gabs/testing/testing.h>

#include <gabs/log.h>

#include "log_capture.hh"

GABS_LOGGER_DECLARE(logger, log_tests);

namespace
{

class capture_fixture
{
      public:
        capture_fixture() : capture(gabs::tests::log_capture::instance())
        {
                capture.reset();
        }

        gabs::tests::log_capture &capture;
};

} // namespace

GABS_SUITE_F(log, capture_fixture);

GABS_TEST_F(log, debug_is_logged)
{
        gabs_log_dbgf(logger, "debug marker");

        GABS_ASSERT_TRUE(fixture->capture.contains("debug marker"));
}

GABS_TEST_F(log, info_is_logged)
{
        gabs_log_inff(logger, "info marker");

        GABS_ASSERT_TRUE(fixture->capture.contains("info marker"));
}

GABS_TEST_F(log, warning_is_logged)
{
        gabs_log_wrnf(logger, "warning marker");

        GABS_ASSERT_TRUE(fixture->capture.contains("warning marker"));
}

GABS_TEST_F(log, error_is_logged)
{
        gabs_log_errf(logger, "error marker");

        GABS_ASSERT_TRUE(fixture->capture.contains("error marker"));
}

GABS_TEST_F(log, arguments_are_formatted)
{
        gabs_log_inff(logger, "int=%d str=%s float=%.2f", 42, "text", 3.5);

        GABS_ASSERT_TRUE(
                fixture->capture.contains("int=42 str=text float=3.50"));
}
