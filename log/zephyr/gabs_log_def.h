
#ifndef GABS_LOG_ZEPHYR_DEF_H__
#define GABS_LOG_ZEPHYR_DEF_H__

#ifndef GABS_LOG_INCLUDE_DEF
#error "gabs_log_def.h should not be included directly"
#endif /* GABS_LOG_INCLUDE_DEF */

#include <zephyr/logging/log.h>

#include <gabs/core/util.h>

GABS_BEGIN_DECL

/* TODO: Could check if logging is enabled */
typedef Z_LOG_INSTANCE_STRUCT *gabs_logger_h;

/** @brief Initialize logging handle */
#define GABS_LOG_ZEPHYR_INIT(module_, inst_) LOG_INSTANCE_PTR(module_, inst_)

#define gabs_log_zephyr__(handle_, level_, ...)                                \
        do {                                                                   \
                /* Needs to be set, just set to  lowest possible.  Filtering   \
                   is primarily done by the level of the logging instance. */  \
                LOG_LEVEL_SET(LOG_LEVEL_DBG);                                  \
                Z_LOG_INSTANCE(level_, handle_, __VA_ARGS__);                  \
        } while (0)

#define gabs_log_dbgf(handle_, ...)                                            \
        gabs_log_zephyr__(handle_, LOG_LEVEL_DBG, __VA_ARGS__)

#define gabs_log_inff(handle_, ...)                                            \
        gabs_log_zephyr__(handle_, LOG_LEVEL_INF, __VA_ARGS__)

#define gabs_log_wrnf(handle_, ...)                                            \
        gabs_log_zephyr__(handle_, LOG_LEVEL_WRN, __VA_ARGS__)

#define gabs_log_errf(handle_, ...)                                            \
        gabs_log_zephyr__(handle_, LOG_LEVEL_ERR, __VA_ARGS__)

GABS_END_DECL

#endif /* GABS_LOG_ZEPHYR_DEF_H__ */
