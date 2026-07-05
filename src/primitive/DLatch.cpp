#include "nfx/silicon/primitive/DLatch.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::primitive
{
    DLatch::DLatch(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinD{ signal::Pin::Descriptor{
              .name = "D", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinEN{ signal::Pin::Descriptor{
              .name = "EN", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinQ{ signal::Pin::Descriptor{
              .name = "Q", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } },
          m_pinNQ{ signal::Pin::Descriptor{
              .name = "NQ", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } },
          m_pinPtrs{ &m_pinD, &m_pinEN, &m_pinQ, &m_pinNQ }
    {
        m_pinD.connect<Level>([this](Level) { compute(); });
        m_pinEN.connect<Level>([this](Level) { compute(); });
    }

    const char* DLatch::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& DLatch::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "DLatch",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "unknown pin name");
        assert(false && "DLatch: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& DLatch::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::D:
                return m_pinD;
            case Terminal::EN:
                return m_pinEN;
            case Terminal::Q:
                return m_pinQ;
            case Terminal::NQ:
                return m_pinNQ;
        }
        internal::runtime::error::log(
            "DLatch",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "invalid terminal");
        assert(false && "DLatch: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> DLatch::pins() const
    {
        return m_pinPtrs;
    }

    void DLatch::reset()
    {
        m_pinQ.drive<Level>(Level::Low);
        m_pinNQ.drive<Level>(Level::High);
    }

    void DLatch::compute()
    {
        const Level en = m_pinEN.read<Level>();
        const Level d = m_pinD.read<Level>();

        if (en == Level::HighZ || en == Level::Low)
        {
            return; // hold
        }

        if (d == Level::HighZ)
        {
            return; // hold
        }

        // EN=High, D valid: transparent
        m_pinQ.drive<Level>(d);
        m_pinNQ.drive<Level>(d == Level::High ? Level::Low : Level::High);
    }
} // namespace nfx::silicon::primitive
