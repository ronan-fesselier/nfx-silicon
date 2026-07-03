#include "nfx/silicon/gate/Not.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::gate
{
    Not::Not(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinA{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinY{ signal::Pin::Descriptor{
              .name = "Y", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } },
          m_pinPtrs{ &m_pinA, &m_pinY }
    {
        m_pinA.connect<Level>([this](Level) { compute(); });
    }

    const char* Not::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& Not::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "Not",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "unknown pin name");
        assert(false && "Not: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& Not::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinA;
            case Terminal::Y:
                return m_pinY;
        }

        internal::runtime::error::log(
            "Not",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "invalid pin");
        assert(false && "Not: invalid terminal");
        return m_pinA;
    }

    std::span<signal::Pin* const> Not::pins() const
    {
        return m_pinPtrs;
    }

    void Not::compute()
    {
        switch (m_pinA.read<Level>())
        {
            case Level::Low:
                m_pinY.drive<Level>(Level::High);
                break;
            case Level::High:
                m_pinY.drive<Level>(Level::Low);
                break;
            case Level::HighZ:
                m_pinY.release();
                break;
        }
    }
} // namespace nfx::silicon::gate
