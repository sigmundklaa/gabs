
#include <memory>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <spdlog/sinks/callback_sink.h>

#include <gabs/log.h>
#include <gabs/log/spdlog.hh>

TEST_CASE("log_spdlog::handle_logs_to_its_logger", "[log_spdlog]")
{
        std::vector<std::string> messages;
        auto sink = std::make_shared<spdlog::sinks::callback_sink_st>(
                [&](const spdlog::details::log_msg &msg) {
                        messages.emplace_back(msg.payload.data(),
                                              msg.payload.size());
                });
        gabs::log::spdlog_logger logger(
                std::make_shared<spdlog::logger>("handle", sink));

        gabs_log_errf(logger, "handle marker %d", 7);

        REQUIRE(messages == std::vector<std::string>{"handle marker 7"});
}
