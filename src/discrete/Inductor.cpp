#include "nfx/silicon/discrete/Inductor.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::discrete
{
    Inductor::Inductor(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinA{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinB{ signal::Pin::Descriptor{
              .name = "B", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinPtrs{ &m_pinA, &m_pinB }
    {}

    const char* Inductor::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& Inductor::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        assert(false && "Inductor: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& Inductor::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinA;
            case Terminal::B:
                return m_pinB;
        }

        assert(false && "Inductor: invalid terminal");
        return m_pinA;
    }

    std::span<signal::Pin* const> Inductor::pins() const
    {
        return m_pinPtrs;
    }

    float Inductor::L() const
    {
        return m_descriptor.henries;
    }
} // namespace nfx::silicon::discrete
