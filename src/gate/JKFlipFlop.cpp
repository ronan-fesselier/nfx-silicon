#include "nfx/silicon/gate/JKFlipFlop.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::gate
{
    JKFlipFlop::JKFlipFlop(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinJ{ signal::Pin::Descriptor{
              .name = "J", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinK{ signal::Pin::Descriptor{
              .name = "K", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinCLK{ signal::Pin::Descriptor{
              .name = "CLK", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinCLR{ signal::Pin::Descriptor{
              .name = "CLR", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinPRE{ signal::Pin::Descriptor{
              .name = "PRE", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinQ{ signal::Pin::Descriptor{
              .name = "Q", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } },
          m_pinNQ{ signal::Pin::Descriptor{
              .name = "NQ", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } },
          m_pinPtrs{ &m_pinJ, &m_pinK, &m_pinCLK, &m_pinCLR, &m_pinPRE, &m_pinQ, &m_pinNQ }
    {
        m_pinCLK.connect<Level>([this](Level level) { onCLK(level); });
        m_pinCLR.connect<Level>([this](Level) { applyAsync(); });
        m_pinPRE.connect<Level>([this](Level) { applyAsync(); });
    }

    const char* JKFlipFlop::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& JKFlipFlop::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "JKFlipFlop",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "unknown pin name");
        assert(false && "JKFlipFlop: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& JKFlipFlop::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::J:
                return m_pinJ;
            case Terminal::K:
                return m_pinK;
            case Terminal::CLK:
                return m_pinCLK;
            case Terminal::CLR:
                return m_pinCLR;
            case Terminal::PRE:
                return m_pinPRE;
            case Terminal::Q:
                return m_pinQ;
            case Terminal::NQ:
                return m_pinNQ;
        }
        internal::runtime::error::log(
            "JKFlipFlop",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "invalid terminal");
        assert(false && "JKFlipFlop: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> JKFlipFlop::pins() const
    {
        return m_pinPtrs;
    }

    void JKFlipFlop::reset()
    {
        m_pinQ.drive<Level>(Level::Low);
        m_pinNQ.drive<Level>(Level::High);
        m_lastCLK = Level::HighZ;
    }

    void JKFlipFlop::onCLK(const Level level)
    {
        const bool risingEdge = (m_lastCLK == Level::Low && level == Level::High);
        m_lastCLK = level;

        if (!risingEdge)
        {
            return;
        }

        const Level clr = m_pinCLR.read<Level>();
        const Level pre = m_pinPRE.read<Level>();

        if (clr == Level::Low && pre == Level::Low)
        {
            m_pinQ.release();
            m_pinNQ.release();
            return;
        }

        if (clr == Level::Low)
        {
            m_pinQ.drive<Level>(Level::Low);
            m_pinNQ.drive<Level>(Level::High);
            return;
        }

        if (pre == Level::Low)
        {
            m_pinQ.drive<Level>(Level::High);
            m_pinNQ.drive<Level>(Level::Low);
            return;
        }

        const Level j = m_pinJ.read<Level>();
        const Level k = m_pinK.read<Level>();

        if (j == Level::High && k == Level::High)
        {
            // toggle: invert current Q
            const Level q = m_pinQ.read<Level>();
            if (q == Level::HighZ)
            {
                return; // indeterminate state, hold
            }
            const Level next = (q == Level::High) ? Level::Low : Level::High;
            m_pinQ.drive<Level>(next);
            m_pinNQ.drive<Level>(q);
            return;
        }

        if (j == Level::High)
        {
            m_pinQ.drive<Level>(Level::High);
            m_pinNQ.drive<Level>(Level::Low);
            return;
        }

        if (k == Level::High)
        {
            m_pinQ.drive<Level>(Level::Low);
            m_pinNQ.drive<Level>(Level::High);
            return;
        }

        // J=Low K=Low or HighZ on either: hold
    }

    void JKFlipFlop::applyAsync()
    {
        const Level clr = m_pinCLR.read<Level>();
        const Level pre = m_pinPRE.read<Level>();

        if (clr == Level::Low && pre == Level::Low)
        {
            m_pinQ.release();
            m_pinNQ.release();
            return;
        }

        if (clr == Level::Low)
        {
            m_pinQ.drive<Level>(Level::Low);
            m_pinNQ.drive<Level>(Level::High);
            return;
        }

        if (pre == Level::Low)
        {
            m_pinQ.drive<Level>(Level::High);
            m_pinNQ.drive<Level>(Level::Low);
            return;
        }
    }
} // namespace nfx::silicon::gate
