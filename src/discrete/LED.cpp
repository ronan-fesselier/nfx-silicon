#include "nfx/silicon/discrete/LED.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::discrete
{
    namespace
    {
        float resolveForwardVoltage(const LED::Descriptor& descriptor)
        {
            switch (descriptor.color)
            {
                case LED::Color::Red:
                    return 1.8f;
                case LED::Color::Green:
                    return 2.1f;
                case LED::Color::Blue:
                    return 3.0f;
                case LED::Color::Yellow:
                    return 2.0f;
                case LED::Color::White:
                    return 3.2f;
                case LED::Color::Custom:
                    return descriptor.forwardVoltage;
            }
            internal::runtime::error::log(
                "LED",
                internal::runtime::error::Level::Critical,
                internal::runtime::error::Kind::Programming,
                "unknown color");
            assert(false && "LED: unknown color");
            return descriptor.forwardVoltage;
        }
    } // namespace

    LED::LED(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_forwardVoltage{ resolveForwardVoltage(descriptor) },
          m_pinAnode{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinCathode{ signal::Pin::Descriptor{
              .name = "K", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinPtrs{ &m_pinAnode, &m_pinCathode }
    {}

    const char* LED::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LED::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LED",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "unknown pin name");
        assert(false && "LED: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LED::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::Anode:
                return m_pinAnode;
            case Terminal::Cathode:
                return m_pinCathode;
        }

        internal::runtime::error::log(
            "LED",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "invalid terminal");
        assert(false && "LED: invalid terminal");
        return m_pinAnode;
    }

    std::span<signal::Pin* const> LED::pins() const
    {
        return m_pinPtrs;
    }

    float LED::forwardVoltage() const
    {
        return m_forwardVoltage;
    }

    float LED::saturationCurrent() const
    {
        return m_descriptor.saturationCurrent;
    }

    float LED::emissionCoefficient() const
    {
        return m_descriptor.emissionCoefficient;
    }
} // namespace nfx::silicon::discrete
