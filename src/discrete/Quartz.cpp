#include "nfx/silicon/discrete/Quartz.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::discrete
{
    Quartz::Quartz(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinX1{ signal::Pin::Descriptor{
              .name = "X1", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinX2{ signal::Pin::Descriptor{
              .name = "X2", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinPtrs{ &m_pinX1, &m_pinX2 }
    {}

    const char* Quartz::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& Quartz::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "Quartz",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Contract,
            "unknown pin name");
        assert(false && "Quartz: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& Quartz::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::X1:
                return m_pinX1;
            case Terminal::X2:
                return m_pinX2;
        }
        internal::runtime::error::log(
            "Quartz",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Contract,
            "invalid terminal");
        assert(false && "Quartz: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> Quartz::pins() const
    {
        return m_pinPtrs;
    }

    float Quartz::frequencyHz() const
    {
        return m_descriptor.frequencyHz;
    }

    const Quartz::Descriptor& Quartz::descriptor() const
    {
        return m_descriptor;
    }
} // namespace nfx::silicon::discrete
