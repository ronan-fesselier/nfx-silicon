#include "Error.h"

#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>

namespace nfx::silicon::internal::runtime::error
{
    namespace
    {
        void formatTimestamp(char* buffer, std::size_t bufferSize)
        {
            const auto now = std::chrono::system_clock::now();
            const auto nowTimeT = std::chrono::system_clock::to_time_t(now);
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            std::tm utcTime{};

#if defined(_WIN32)
            gmtime_s(&utcTime, &nowTimeT);
#else
            gmtime_r(&nowTimeT, &utcTime);
#endif

            std::strftime(buffer, bufferSize, "%Y-%m-%dT%H:%M:%S", &utcTime);
            std::snprintf(
                buffer + std::strlen(buffer),
                bufferSize - std::strlen(buffer),
                ".%03lldZ",
                static_cast<long long>(ms.count()));
        }

        const char* toString(Level level)
        {
            switch (level)
            {
                case Level::Trace:
                    return "TRACE";
                case Level::Debug:
                    return "DEBUG";
                case Level::Info:
                    return "INFO";
                case Level::Warning:
                    return "WARNING";
                case Level::Critical:
                    return "CRITICAL";
                case Level::Fatal:
                    return "FATAL";
            }
            return "UNKNOWN";
        }

        const char* toString(Kind kind)
        {
            switch (kind)
            {
                case Kind::Programming:
                    return "PROGRAMMING";
                case Kind::Contract:
                    return "CONTRACT";
                case Kind::Invariant:
                    return "INVARIANT";
                case Kind::Timing:
                    return "TIMING";
                case Kind::Signal:
                    return "SIGNAL";
                case Kind::Configuration:
                    return "CONFIGURATION";
            }
            return "UNKNOWN";
        }
    } // namespace

    void log(std::string_view module, Level level, std::string_view kind, std::string_view message)
    {
        char timestamp[32]{};
        formatTimestamp(timestamp, sizeof(timestamp));

        std::fprintf(
            stderr,
            "%s [%.*s] %s(%.*s): %.*s\n",
            timestamp,
            static_cast<int>(module.size()),
            module.data(),
            toString(level),
            static_cast<int>(kind.size()),
            kind.data(),
            static_cast<int>(message.size()),
            message.data());
    }

    void log(std::string_view module, Level level, Kind kind, std::string_view message)
    {
        log(module, level, toString(kind), message);
    }
} // namespace nfx::silicon::internal::runtime::error
