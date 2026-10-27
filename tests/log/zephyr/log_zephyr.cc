
#include <zephyr/logging/log.h>
#include <zephyr/ztest.h>

#include <gabs/log.h>

#include "../log_capture.hh"

LOG_MODULE_REGISTER(gabs_test_module, LOG_LEVEL_DBG);
LOG_INSTANCE_REGISTER(gabs_test_module, inst, LOG_LEVEL_DBG);
GABS_LOGGER_ZEPHYR_DECLARE(logger, gabs_test_module, inst);

ZTEST(log_zephyr, logger_logs_to_its_instance)
{
        auto &capture = gabs::tests::log_capture::instance();

        capture.reset();
        gabs_log_inff(logger, "instance marker");

        zassert_true(capture.contains("gabs_test_module.inst"));
        zassert_true(capture.contains("instance marker"));
}

ZTEST_SUITE(log_zephyr, nullptr, nullptr, nullptr, nullptr, nullptr);
