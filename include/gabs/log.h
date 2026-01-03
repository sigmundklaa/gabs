/**
 * The log implementation must provide a `gabs_log_def.h` header, which defines
 * the following macros/functions:
 * - `gabs_log_dbgf(fmt_, ...)`
 * - `gabs_log_inff(fmt_, ...)`
 * - `gabs_log_wrnf(fmt_, ...)`
 * - `gabs_log_errf(fmt_, ...)`
 */

#ifndef GABS_LOG_H__
#define GABS_LOG_H__

#define GABS_LOG_INCLUDE_DEF
#include <gabs_log_def.h>
#undef GABS_LOG_INCLUDE_DEF

#endif /* GABS_LOG_H__ */
