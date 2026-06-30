#include "nfx/silicon/discrete/Capacitor.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::discrete
{
    Capacitor::Capacitor(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinA{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinB{ signal::Pin::Descriptor{
              .name = "B", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinPtrs{ &m_pinA, &m_pinB }
    {}

    const char* Capacitor::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& Capacitor::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        assert(false && "Capacitor: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& Capacitor::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinA;
            case Terminal::B:
                return m_pinB;
        }

        assert(false && "Capacitor: invalid terminal");
        return m_pinA;
    }

    std::span<signal::Pin* const> Capacitor::pins() const
    {
        return m_pinPtrs;
    }

    float Capacitor::C() const
    {
        return m_descriptor.farads;
    }
} // namespace nfx::silicon::discrete
