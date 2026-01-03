
#ifndef GABS_LOG_SPDLOG_DEF_H__
#define GABS_LOG_SPDLOG_DEF_H__

#include <gabs/core/util.h>

GABS_BEGIN_DECL

/* Can't expose actual spdlog level, as we can't include the header file in
 * case this is used by a C library. Thus we just use an internal level
 * that is converted in the C++ code. */
enum gabs_log_spdlog_level__ {
        GABS_LOG_SPDLOG_DEBUG__,
        GABS_LOG_SPDLOG_INFO__,
        GABS_LOG_SPDLOG_WARN__,
        GABS_LOG_SPDLOG_ERROR__
};

/**
 * @brief Actual implementation for writing log message to spdlog
 * (implementation in C++).
 */
void gabs_log_spdlog_logf_impl__(enum gabs_log_spdlog_level__ level,
                                 const char *file, const char *func, int line,
                                 const char *fmt, ...);

#define gabs_log_spdlog_logf__(level_, ...)                                    \
        gabs_log_spdlog_logf_impl__(level_, __FILE__, __func__, __LINE__,      \
                                    __VA_ARGS__)

#define gabs_log_dbgf(...)                                                     \
        gabs_log_spdlog_logf__(GABS_LOG_SPDLOG_DEBUG__, __VA_ARGS__)

#define gabs_log_inff(...)                                                     \
        gabs_log_spdlog_logf__(GABS_LOG_SPDLOG_INFO__, __VA_ARGS__)

#define gabs_log_wrnf(...)                                                     \
        gabs_log_spdlog_logf__(GABS_LOG_SPDLOG_WRN__, __VA_ARGS__)

#define gabs_log_errf(...)                                                     \
        gabs_log_spdlog_logf__(GABS_LOG_SPDLOG_ERR__, __VA_ARGS__)

GABS_END_DECL

#endif /* GABS_LOG_SPDLOG_DEF_H__ */
