
#ifndef GABS_LOG_DISABLED_DEF_H__
#define GABS_LOG_DISABLED_DEF_H__

#include <gabs/core/util.h>
#include <gabs/core/handle.h>

#define gabs_disabled_logf__(handle_, fmt_, ...)                               \
        do {                                                                   \
                GABS_UNUSED(handle_);                                          \
                GABS_UNUSED(fmt_);                                             \
        } while (0)

#define gabs_log_dbgf(...) gabs_disabled_logf__(__VA_ARGS__)
#define gabs_log_inff(...) gabs_disabled_logf__(__VA_ARGS__)
#define gabs_log_wrnf(...) gabs_disabled_logf__(__VA_ARGS__)
#define gabs_log_errf(...) gabs_disabled_logf__(__VA_ARGS__)

#endif /* GABS_LOG_DISABLED_DEF_H__ */
