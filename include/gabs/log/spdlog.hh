
#ifndef GABS_LOG_SPDLOG_HH__
#define GABS_LOG_SPDLOG_HH__

#include <gabs/cc/include_guard.hh>

#include <spdlog/spdlog.h>

#include <gabs/log.h>

namespace gabs::log
{

/**
 * @brief Simple wrapper around an spdlog::logger object, that provides a
 * handle that can be passed around to C-code.
 */
class spdlog_logger
{
      public:
        spdlog_logger(const std::shared_ptr<::spdlog::logger> &logger =
                              spdlog::default_logger())
                : logger_(logger)
        {
        }

        const std::shared_ptr<::spdlog::logger> &logger() const
        {
                return logger_;
        }

        /* Internal handle type is `gabs_handle`, and `gabs_log_def.h` defines
         * `gabs_logger_h` as `const gabs_handle*`. */
        ::gabs_logger_h handle() const
        {
                return &handle_;
        }

        static spdlog_logger *from_handle(::gabs_logger_h h)
        {
                return gabs_container_of(h, spdlog_logger, handle_);
        }

      private:
        std::shared_ptr<::spdlog::logger> logger_;
        ::gabs_handle handle_;
};

}; // namespace gabs::log

#endif /* GABS_LOG_SPDLOG_HH__ */
