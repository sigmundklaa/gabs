
#ifndef GABS_LOG_SPDLOG_HH__
#define GABS_LOG_SPDLOG_HH__

#include <gabs/cc/include_guard.hh>

#include <spdlog/spdlog.h>

#include <gabs/log.h>
#include <gabs/cc/handle.hh>

namespace gabs::log
{

/**
 * @brief Simple wrapper around an spdlog::logger object, that provides a
 * handle that can be passed around to C-code.
 */
class spdlog_logger
        : public core::handle_wrapper<
                  spdlog_logger, core::simple_handle_trait<::gabs_logger_h>>
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

      private:
        std::shared_ptr<::spdlog::logger> logger_;
};

}; // namespace gabs::log

#endif /* GABS_LOG_SPDLOG_HH__ */
