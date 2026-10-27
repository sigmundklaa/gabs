
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <spdlog/logger.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/spdlog.h>

#include "log_capture.hh"

namespace
{

class spdlog_capture : public gabs::tests::log_capture
{
      public:
        spdlog_capture()
        {
                auto sink = std::make_shared<spdlog::sinks::callback_sink_mt>(
                        [this](const spdlog::details::log_msg &msg) {
                                std::lock_guard<std::mutex> guard(lock_);

                                messages_.emplace_back(msg.payload.data(),
                                                       msg.payload.size());
                        });
                auto logger =
                        std::make_shared<spdlog::logger>("gabs-tests", sink);

                logger->set_level(spdlog::level::trace);
                spdlog::set_default_logger(logger);
        }

        void reset() override
        {
                std::lock_guard<std::mutex> guard(lock_);

                messages_.clear();
        }

        bool contains(std::string_view needle) override
        {
                std::lock_guard<std::mutex> guard(lock_);

                for (const auto &msg : messages_) {
                        if (msg.find(needle) != std::string::npos) {
                                return true;
                        }
                }

                return false;
        }

      private:
        std::mutex lock_;
        std::vector<std::string> messages_;
};

} // namespace

gabs::tests::log_capture &gabs::tests::log_capture::instance()
{
        static spdlog_capture capture;

        return capture;
}
