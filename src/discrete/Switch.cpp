#include "nfx/silicon/discrete/Switch.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::discrete
{
    namespace
    {
        constexpr float kOpenResistance = 1e12f; ///< SPICE convention for an open contact
    } // namespace

    Switch::Switch(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_state{ descriptor.initialState },
          m_pinA{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinB{ signal::Pin::Descriptor{
              .name = "B", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinPtrs{ &m_pinA, &m_pinB }
    {}

    const char* Switch::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& Switch::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        assert(false && "Switch: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& Switch::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinA;
            case Terminal::B:
                return m_pinB;
        }

        assert(false && "Switch: invalid terminal");
        return m_pinA;
    }

    std::span<signal::Pin* const> Switch::pins() const
    {
        return m_pinPtrs;
    }

    void Switch::close()
    {
        m_state = State::Closed;
    }

    void Switch::open()
    {
        m_state = State::Open;
    }

    Switch::State Switch::state() const
    {
        return m_state;
    }

    float Switch::R() const
    {
        return m_state == State::Closed ? 0.0f : kOpenResistance;
    }
} // namespace nfx::silicon::discrete
