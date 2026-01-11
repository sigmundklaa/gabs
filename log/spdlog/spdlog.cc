
#include <string>
#include <stdarg.h>
#include <assert.h>

#include <spdlog/spdlog.h>

#include <gabs/log/spdlog.hh>
#include <gabs/cc/defer.hh>

namespace
{

using namespace gabs;

std::string vasprintf_like(const char *fmt, va_list &va)
{
        std::size_t size;
        va_list va_dry;
        va_copy(va_dry, va);
        util::defer _([&]() { va_end(va_dry); });

        size = ::vsnprintf(nullptr, 0, fmt, va_dry);
        if (size < 0) {
                assert(0);
                return "";
        }

        std::string str;
        str.resize(size + 1);

        auto ret = ::vsnprintf(str.data(), size + 1, fmt, va);
        assert(ret == size);

        return str;
}

spdlog::level::level_enum convert_level(enum gabs_log_spdlog_level__ level)
{
        switch (level) {
        case GABS_LOG_SPDLOG_DEBUG__:
                return spdlog::level::debug;
        case GABS_LOG_SPDLOG_INFO__:
                return spdlog::level::info;
        case GABS_LOG_SPDLOG_WARN__:
                return spdlog::level::warn;
        case GABS_LOG_SPDLOG_ERROR__:
                return spdlog::level::err;
        }
}

}; // namespace

void gabs_log_spdlog_impl__(const gabs_logger_h *handle,
                            enum gabs_log_spdlog_level__ level,
                            const char *file, const char *func, int line,
                            const char *fmt, ...)
{
        va_list va;
        va_start(va, fmt);
        util::defer _([&]() { va_end(va); });

        auto msg = vasprintf_like(fmt, va);
        auto logger = gabs::log::spdlog_logger::from_handle(handle)->logger();
        auto spdlevel = convert_level(level);
        auto loc = spdlog::source_loc{file, line, func};

        if (logger->should_log(spdlevel)) {
                logger->log(loc, spdlevel, "{}", msg);
        }
}
