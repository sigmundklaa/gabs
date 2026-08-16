
#ifndef GABS_TESTS_LOG_CAPTURE_H__
#define GABS_TESTS_LOG_CAPTURE_H__

#include <stdbool.h>

#include <gabs/core/util.h>
#include <gabs/log.h>

GABS_BEGIN_DECL

/**
 * @brief Discard any log output captured since the last reset.
 */
void gabs_test_log_capture_reset(void);

/**
 * @brief Handle to pass to gabs_log_* calls under test; its output ends up
 * somewhere gabs_test_log_capture_contains() can see it.
 */
const gabs_logger_h *gabs_test_log_capture_handle(void);

/**
 * @brief Check whether @p needle appears anywhere in the log output
 * captured since the last gabs_test_log_capture_reset() call.
 */
bool gabs_test_log_capture_contains(const char *needle);

GABS_END_DECL

#endif /* GABS_TESTS_LOG_CAPTURE_H__ */
