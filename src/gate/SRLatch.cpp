#include "nfx/silicon/gate/SRLatch.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::gate
{
    SRLatch::SRLatch(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinS{ signal::Pin::Descriptor{
              .name = "S", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinR{ signal::Pin::Descriptor{
              .name = "R", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinQ{ signal::Pin::Descriptor{
              .name = "Q", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } },
          m_pinNQ{ signal::Pin::Descriptor{
              .name = "NQ", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } },
          m_pinPtrs{ &m_pinS, &m_pinR, &m_pinQ, &m_pinNQ }
    {
        m_pinS.connect<Level>([this](Level) { compute(); });
        m_pinR.connect<Level>([this](Level) { compute(); });
    }

    const char* SRLatch::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& SRLatch::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "SRLatch",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "unknown pin name");
        assert(false && "SRLatch: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& SRLatch::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::S:
                return m_pinS;
            case Terminal::R:
                return m_pinR;
            case Terminal::Q:
                return m_pinQ;
            case Terminal::NQ:
                return m_pinNQ;
        }
        internal::runtime::error::log(
            "SRLatch",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "invalid terminal");
        assert(false && "SRLatch: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> SRLatch::pins() const
    {
        return m_pinPtrs;
    }

    void SRLatch::reset()
    {
        m_pinQ.drive<Level>(Level::Low);
        m_pinNQ.drive<Level>(Level::High);
    }

    void SRLatch::compute()
    {
        const Level s = m_pinS.read<Level>();
        const Level r = m_pinR.read<Level>();

        if (s == Level::HighZ || r == Level::HighZ)
        {
            return; // hold
        }

        if (s == Level::High && r == Level::High)
        {
            m_pinQ.release();
            m_pinNQ.release();
            return;
        }

        if (s == Level::High)
        {
            m_pinQ.drive<Level>(Level::High);
            m_pinNQ.drive<Level>(Level::Low);
            return;
        }

        if (r == Level::High)
        {
            m_pinQ.drive<Level>(Level::Low);
            m_pinNQ.drive<Level>(Level::High);
            return;
        }

        // S=Low, R=Low: hold, no change
    }
} // namespace nfx::silicon::gate
