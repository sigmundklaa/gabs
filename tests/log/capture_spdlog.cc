
#include "log_capture.h"

#include <memory>
#include <string>
#include <vector>

#include <spdlog/logger.h>
#include <spdlog/sinks/callback_sink.h>

#include <gabs/log/spdlog.hh>

namespace
{

std::vector<std::string> &captured_messages()
{
        static std::vector<std::string> messages;
        return messages;
}

gabs::log::spdlog_logger &capture_logger()
{
        static std::unique_ptr<gabs::log::spdlog_logger> logger = [] {
                auto sink = std::make_shared<spdlog::sinks::callback_sink_mt>(
                        [](const spdlog::details::log_msg &msg) {
                                captured_messages().emplace_back(
                                        msg.payload.data(),
                                        msg.payload.size());
                        });
                auto spd_logger = std::make_shared<spdlog::logger>(
                        "gabs-test-log-capture", sink);
                spd_logger->set_level(spdlog::level::trace);

                return std::make_unique<gabs::log::spdlog_logger>(spd_logger);
        }();

        return *logger;
}

} // namespace

void gabs_test_log_reset(void)
{
        captured_messages().clear();
}

const gabs_logger_h *gabs_test_log_handle(void)
{
        return capture_logger();
}

bool gabs_test_log_contains(const char *needle)
{
        for (const auto &msg : captured_messages()) {
                if (msg.find(needle) != std::string::npos) {
                        return true;
                }
        }

        return false;
}
