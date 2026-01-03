
#ifndef GABS_LOG_ZEPHYR_DEF_H__
#define GABS_LOG_ZEPHYR_DEF_H__

#include <zephyr/logging/log.h>

#include <gabs/core/util.h>

GABS_BEGIN_DECL

#define gabs_log_dbgf(...) LOG_DBG(__VA_ARGS__)
#define gabs_log_inff(...) LOG_INF(__VA_ARGS__)
#define gabs_log_wrnf(...) LOG_WRN(__VA_ARGS__)
#define gabs_log_errf(...) LOG_ERR(__VA_ARGS__)

GABS_END_DECL

#endif /* GABS_LOG_ZEPHYR_DEF_H__ */
