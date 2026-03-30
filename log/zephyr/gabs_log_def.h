
#ifndef GABS_LOG_ZEPHYR_DEF_H__
#define GABS_LOG_ZEPHYR_DEF_H__

#ifndef GABS_LOG_INCLUDE_DEF
#error "gabs_log_def.h should not be included directly"
#endif /* GABS_LOG_INCLUDE_DEF */

#include <zephyr/logging/log.h>

#include <gabs/core/util.h>

GABS_BEGIN_DECL

struct gabs_log_zephyr_data {
        LOG_INSTANCE_PTR_DECLARE(ptr);
        gabs_logger_h handle;
};

/**
 * @brief Initialize logging handle
 *
 * This assumes that both `LOG_MODULE_DECLARE`/`LOG_MODULE_REGISTER` and
 * `LOG_INSTANCE_REGISTER` has been invoked already. This can be used by
 * application code to create a logger that directs its output to a Zephyr
 * instance.
 */
#define GABS_LOGGER_ZEPHYR_DECLARE(handle_, module_, inst_)                    \
        static struct gabs_log_zephyr_data handle_##data__ = {                 \
                LOG_INSTANCE_PTR(module_, inst_),                              \
        };                                                                     \
        static const gabs_logger_h *handle_ = &handle_##data__.handle

#define GABS_LOGGER_ZEPHYR_GET(handle_)                                        \
        gabs_container_of(handle_, struct gabs_log_zephyr_data, handle)->ptr

/**
 * @brief Create logger in gabs default namespace
 */
#define GABS_LOGGER_DECLARE(handle_, name_)                                    \
        LOG_MODULE_DECLARE(gabs, CONFIG_GABS_LOG_LEVEL);                       \
        LOG_INSTANCE_REGISTER(gabs, name_, LOG_LEVEL_DBG);                     \
        GABS_LOGGER_ZEPHYR_DECLARE(handle_, gabs, name_)

#define gabs_log_zephyr__(handle_, level_, ...)                                \
        do {                                                                   \
                if (!handle_) {                                                \
                        break;                                                 \
                }                                                              \
                /* Needs to be set, just set to  lowest possible.  Filtering   \
                   is primarily done by the level of the logging instance. */  \
                LOG_LEVEL_SET(LOG_LEVEL_DBG);                                  \
                Z_LOG_INSTANCE(level_, GABS_LOGGER_ZEPHYR_GET(handle_),        \
                               __VA_ARGS__);                                   \
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
