#include "nfx/silicon/discrete/Diode.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::discrete
{
    Diode::Diode(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinAnode{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinCathode{ signal::Pin::Descriptor{
              .name = "K", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinPtrs{ &m_pinAnode, &m_pinCathode }
    {}

    const char* Diode::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& Diode::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "Diode",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "unknown pin name");
        assert(false && "Diode: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& Diode::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinAnode;

            case Terminal::K:
                return m_pinCathode;
        }

        internal::runtime::error::log(
            "Diode",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "invalid terminal");
        assert(false && "Diode: invalid terminal");
        return m_pinAnode;
    }

    std::span<signal::Pin* const> Diode::pins() const
    {
        return m_pinPtrs;
    }

    float Diode::forwardVoltage() const
    {
        return m_descriptor.forwardVoltage;
    }

    float Diode::saturationCurrent() const
    {
        return m_descriptor.saturationCurrent;
    }

    float Diode::emissionCoefficient() const
    {
        return m_descriptor.emissionCoefficient;
    }
} // namespace nfx::silicon::discrete
