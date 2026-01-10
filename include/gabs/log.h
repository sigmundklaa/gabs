/**
 * The log implementation must provide a `gabs_log_def.h` header, which defines
 * the following types/macros/functions:
 * - `gabs_log_dbgf(const gabs_logger_h *handle_, const char* fmt_, ...)`
 * - `gabs_log_inff(const gabs_logger_h *handle_, const char* fmt_, ...)`
 * - `gabs_log_wrnf(const gabs_logger_h *handle_, const char* fmt_, ...)`
 * - `gabs_log_errf(const gabs_logger_h *handle_, const char* fmt_, ...)`
 */

#ifndef GABS_LOG_H__
#define GABS_LOG_H__

#include <gabs/core/handle.h>

typedef gabs_handle gabs_logger_h;

#define GABS_LOG_INCLUDE_DEF
#include <gabs_log_def.h>
#undef GABS_LOG_INCLUDE_DEF

#endif /* GABS_LOG_H__ */
