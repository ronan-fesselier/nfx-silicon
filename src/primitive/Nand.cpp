#include "nfx/silicon/primitive/Nand.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::primitive
{
    Nand::Nand(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinA{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinB{ signal::Pin::Descriptor{
              .name = "B", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinY{ signal::Pin::Descriptor{
              .name = "Y", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } },
          m_pinPtrs{ &m_pinA, &m_pinB, &m_pinY }
    {
        m_pinA.connect<Level>([this](Level) { compute(); });
        m_pinB.connect<Level>([this](Level) { compute(); });
    }

    const char* Nand::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& Nand::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "Nand",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "unknown pin name");
        assert(false && "Nand: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& Nand::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinA;
            case Terminal::B:
                return m_pinB;
            case Terminal::Y:
                return m_pinY;
        }

        internal::runtime::error::log(
            "Nand",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "invalid terminal");
        assert(false && "Nand: invalid terminal");
        return m_pinA;
    }

    std::span<signal::Pin* const> Nand::pins() const
    {
        return m_pinPtrs;
    }

    void Nand::compute()
    {
        const Level a = m_pinA.read<Level>();
        const Level b = m_pinB.read<Level>();

        if (a == Level::HighZ || b == Level::HighZ)
        {
            m_pinY.release();
            return;
        }

        m_pinY.drive<Level>((a == Level::High && b == Level::High) ? Level::Low : Level::High);
    }
} // namespace nfx::silicon::primitive
