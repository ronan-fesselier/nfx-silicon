#include "nfx/silicon/discrete/PushButton.h"

#include "detail/ContactResistance.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::discrete
{
    PushButton::PushButton(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_isPressed{ false },
          m_pinA{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinB{ signal::Pin::Descriptor{
              .name = "B", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } },
          m_pinPtrs{ &m_pinA, &m_pinB }
    {}

    const char* PushButton::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& PushButton::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "PushButton",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "unknown pin name");
        assert(false && "PushButton: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& PushButton::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinA;
            case Terminal::B:
                return m_pinB;
        }

        internal::runtime::error::log(
            "PushButton",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "invalid terminal");
        assert(false && "PushButton: invalid terminal");
        return m_pinA;
    }

    std::span<signal::Pin* const> PushButton::pins() const
    {
        return m_pinPtrs;
    }

    void PushButton::press()
    {
        m_isPressed = true;
    }

    void PushButton::release()
    {
        m_isPressed = false;
    }

    bool PushButton::isPressed() const
    {
        return m_isPressed;
    }

    float PushButton::R() const
    {
        const bool closed = (m_descriptor.contactType == ContactType::NormallyOpen) ? m_isPressed : !m_isPressed;
        return closed ? 0.0f : detail::kOpenResistance;
    }
} // namespace nfx::silicon::discrete
