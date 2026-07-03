#include "nfx/silicon/gate/DFlipFlop.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::gate
{
    DFlipFlop::DFlipFlop(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinD{ signal::Pin::Descriptor{
              .name = "D", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
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
          m_pinPtrs{ &m_pinD, &m_pinCLK, &m_pinCLR, &m_pinPRE, &m_pinQ, &m_pinNQ }
    {
        m_pinCLK.connect<Level>([this](Level level) { onCLK(level); });
        m_pinCLR.connect<Level>([this](Level) { applyAsync(); });
        m_pinPRE.connect<Level>([this](Level) { applyAsync(); });
    }

    const char* DFlipFlop::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& DFlipFlop::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "DFlipFlop",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "unknown pin name");
        assert(false && "DFlipFlop: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& DFlipFlop::pin(Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::D:
                return m_pinD;
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
            "DFlipFlop",
            internal::runtime::error::Level::Critical,
            internal::runtime::error::Kind::Programming,
            "invalid terminal");
        assert(false && "DFlipFlop: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> DFlipFlop::pins() const
    {
        return m_pinPtrs;
    }

    void DFlipFlop::reset()
    {
        m_pinQ.drive<Level>(Level::Low);
        m_pinNQ.drive<Level>(Level::High);
        m_lastCLK = Level::HighZ;
    }

    void DFlipFlop::onCLK(const Level level)
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

        const Level d = m_pinD.read<Level>();
        if (d == Level::HighZ)
        {
            return; // hold
        }

        m_pinQ.drive<Level>(d);
        m_pinNQ.drive<Level>(d == Level::High ? Level::Low : Level::High);
    }

    void DFlipFlop::applyAsync()
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
