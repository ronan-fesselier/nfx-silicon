#include "nfx/silicon/gate/sn74xxx/LS74.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::gate::sn74
{
    namespace
    {
        using Kind = internal::runtime::error::Kind;

        signal::Pin makeInput(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } };
        }

        signal::Pin makePower(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Input } };
        }
    } // namespace

    LS74::LS74(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_ff1{ DFlipFlop::Descriptor{ .name = "FF1" } },
          m_ff2{ DFlipFlop::Descriptor{ .name = "FF2" } },
          m_pin1CLR{ makeInput("1CLR") },
          m_pin1D{ makeInput("1D") },
          m_pin1CLK{ makeInput("1CLK") },
          m_pin1PRE{ makeInput("1PRE") },
          m_pin1Q{ makeInput("1Q") },
          m_pin1NQ{ makeInput("1NQ") },
          m_pinGND{ makePower("GND") },
          m_pin2NQ{ makeInput("2NQ") },
          m_pin2Q{ makeInput("2Q") },
          m_pin2PRE{ makeInput("2PRE") },
          m_pin2CLK{ makeInput("2CLK") },
          m_pin2D{ makeInput("2D") },
          m_pin2CLR{ makeInput("2CLR") },
          m_pinVCC{ makePower("VCC") },
          m_pinPtrs{ &m_pin1CLR, &m_pin1D, &m_pin1CLK, &m_pin1PRE, &m_pin1Q, &m_pin1NQ,  &m_pinGND,
                     &m_pin2NQ,  &m_pin2Q, &m_pin2PRE, &m_pin2CLK, &m_pin2D, &m_pin2CLR, &m_pinVCC }
    {
        m_pin1CLR.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff1.pin(DFlipFlop::Terminal::CLR).drive<Level>(l);
        });
        m_pin1D.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff1.pin(DFlipFlop::Terminal::D).drive<Level>(l);
        });
        m_pin1CLK.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff1.pin(DFlipFlop::Terminal::CLK).drive<Level>(l);
        });
        m_pin1PRE.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff1.pin(DFlipFlop::Terminal::PRE).drive<Level>(l);
        });
        m_ff1.pin(DFlipFlop::Terminal::Q).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pin1Q.drive<Level>(l);
        });
        m_ff1.pin(DFlipFlop::Terminal::NQ).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pin1NQ.drive<Level>(l);
        });

        m_pin2CLR.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff2.pin(DFlipFlop::Terminal::CLR).drive<Level>(l);
        });
        m_pin2D.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff2.pin(DFlipFlop::Terminal::D).drive<Level>(l);
        });
        m_pin2CLK.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff2.pin(DFlipFlop::Terminal::CLK).drive<Level>(l);
        });
        m_pin2PRE.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff2.pin(DFlipFlop::Terminal::PRE).drive<Level>(l);
        });
        m_ff2.pin(DFlipFlop::Terminal::Q).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pin2Q.drive<Level>(l);
        });
        m_ff2.pin(DFlipFlop::Terminal::NQ).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pin2NQ.drive<Level>(l);
        });

        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* LS74::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LS74::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LS74", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "LS74: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LS74::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::CLR1:
                return m_pin1CLR;
            case Terminal::D1:
                return m_pin1D;
            case Terminal::CLK1:
                return m_pin1CLK;
            case Terminal::PRE1:
                return m_pin1PRE;
            case Terminal::Q1:
                return m_pin1Q;
            case Terminal::NQ1:
                return m_pin1NQ;
            case Terminal::GND:
                return m_pinGND;
            case Terminal::NQ2:
                return m_pin2NQ;
            case Terminal::Q2:
                return m_pin2Q;
            case Terminal::PRE2:
                return m_pin2PRE;
            case Terminal::CLK2:
                return m_pin2CLK;
            case Terminal::D2:
                return m_pin2D;
            case Terminal::CLR2:
                return m_pin2CLR;
            case Terminal::VCC:
                return m_pinVCC;
        }
        internal::runtime::error::log(
            "LS74", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "LS74: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> LS74::pins() const
    {
        return m_pinPtrs;
    }

    void LS74::reset()
    {
        m_ff1.reset();
        m_ff2.reset();
    }

    void LS74::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (!wasPowered && m_powered)
        {
            m_ff1.reset();
            m_ff2.reset();
        }
        else if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "LS74",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: outputs held at HighZ");
            m_pin1Q.release();
            m_pin1NQ.release();
            m_pin2Q.release();
            m_pin2NQ.release();
        }
    }
} // namespace nfx::silicon::gate::sn74
