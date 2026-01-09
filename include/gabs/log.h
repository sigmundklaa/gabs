/**
 * The log implementation must provide a `gabs_log_def.h` header, which defines
 * the following types/macros/functions:
 * - `gabs_logger_h`
 * - `gabs_log_dbgf(gabs_logger_h handle_, const char* fmt_, ...)`
 * - `gabs_log_inff(gabs_logger_h handle_, const char* fmt_, ...)`
 * - `gabs_log_wrnf(gabs_logger_h handle_, const char* fmt_, ...)`
 * - `gabs_log_errf(gabs_logger_h handle_, const char* fmt_, ...)`
 */

#ifndef GABS_LOG_H__
#define GABS_LOG_H__

#define GABS_LOG_INCLUDE_DEF
#include <gabs_log_def.h>
#undef GABS_LOG_INCLUDE_DEF

#endif /* GABS_LOG_H__ */
