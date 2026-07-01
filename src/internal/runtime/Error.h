#pragma once

#include <string_view>

namespace nfx::silicon::internal::runtime
{
    namespace error
    {
        enum class Level
        {
            Trace,
            Debug,
            Info,
            Warning,
            Critical,
            Fatal
        };

        enum class Kind
        {
            Programming,
            Contract,
            Invariant,
            Timing,
            Signal,
            Configuration
        };

        void log(std::string_view module, Level level, Kind kind, std::string_view message);
        void log(std::string_view module, Level level, std::string_view kind, std::string_view message);
    } // namespace error
} // namespace nfx::silicon::internal::runtime
