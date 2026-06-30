#include "nfx/silicon/discrete/Resistor.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::discrete
{
    Resistor::Resistor(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinA{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinB{ signal::Pin::Descriptor{
              .name = "B", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinPtrs{ &m_pinA, &m_pinB }
    {}

    const char* Resistor::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& Resistor::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        assert(false && "Resistor: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& Resistor::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinA;
            case Terminal::B:
                return m_pinB;
        }

        assert(false && "Resistor: invalid terminal");
        return m_pinA;
    }

    std::span<signal::Pin* const> Resistor::pins() const
    {
        return m_pinPtrs;
    }

    float Resistor::R() const
    {
        return m_descriptor.ohms;
    }
} // namespace nfx::silicon::discrete
