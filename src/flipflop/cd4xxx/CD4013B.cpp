#include "nfx/silicon/flipflop/cd4xxx/CD4013B.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::flipflop::cd4xxx
{
    namespace
    {
        using Kind = internal::runtime::error::Kind;

        signal::Pin makeInput(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } };
        }

        signal::Pin makeOutput(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } };
        }

        signal::Pin makePower(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Input } };
        }

        // SET and RESET on CD4013B are active High; DFlipFlop PRE/CLR are active Low.
        // Invert: High input -> Low to primitive, Low input -> High to primitive.
        Level invert(const Level l)
        {
            if (l == Level::High)
            {
                return Level::Low;
            }
            if (l == Level::Low)
            {
                return Level::High;
            }
            return Level::HighZ;
        }
    } // namespace

    CD4013B::CD4013B(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_ff1{ primitive::DFlipFlop::Descriptor{ .name = "FF1" } },
          m_ff2{ primitive::DFlipFlop::Descriptor{ .name = "FF2" } },
          m_pinQ1{ makeOutput("Q1") },
          m_pinNQ1{ makeOutput("/Q1") },
          m_pinCLOCK1{ makeInput("CLOCK1") },
          m_pinRESET1{ makeInput("RESET1") },
          m_pinD1{ makeInput("D1") },
          m_pinSET1{ makeInput("SET1") },
          m_pinVSS{ makePower("VSS") },
          m_pinSET2{ makeInput("SET2") },
          m_pinD2{ makeInput("D2") },
          m_pinRESET2{ makeInput("RESET2") },
          m_pinCLOCK2{ makeInput("CLOCK2") },
          m_pinNQ2{ makeOutput("/Q2") },
          m_pinQ2{ makeOutput("Q2") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinQ1,   &m_pinNQ1, &m_pinCLOCK1, &m_pinRESET1, &m_pinD1,  &m_pinSET1, &m_pinVSS,
                     &m_pinSET2, &m_pinD2,  &m_pinRESET2, &m_pinCLOCK2, &m_pinNQ2, &m_pinQ2,   &m_pinVDD }
    {
        // Flip-flop 1 wiring
        m_pinSET1.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff1.pin(primitive::DFlipFlop::Terminal::PRE).drive<Level>(invert(l));
        });
        m_pinRESET1.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff1.pin(primitive::DFlipFlop::Terminal::CLR).drive<Level>(invert(l));
        });
        m_pinD1.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff1.pin(primitive::DFlipFlop::Terminal::D).drive<Level>(l);
        });
        m_pinCLOCK1.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff1.pin(primitive::DFlipFlop::Terminal::CLK).drive<Level>(l);
        });
        m_ff1.pin(primitive::DFlipFlop::Terminal::Q).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinQ1.drive<Level>(l);
        });
        m_ff1.pin(primitive::DFlipFlop::Terminal::NQ).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinNQ1.drive<Level>(l);
        });

        // Flip-flop 2 wiring
        m_pinSET2.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff2.pin(primitive::DFlipFlop::Terminal::PRE).drive<Level>(invert(l));
        });
        m_pinRESET2.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff2.pin(primitive::DFlipFlop::Terminal::CLR).drive<Level>(invert(l));
        });
        m_pinD2.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff2.pin(primitive::DFlipFlop::Terminal::D).drive<Level>(l);
        });
        m_pinCLOCK2.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_ff2.pin(primitive::DFlipFlop::Terminal::CLK).drive<Level>(l);
        });
        m_ff2.pin(primitive::DFlipFlop::Terminal::Q).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinQ2.drive<Level>(l);
        });
        m_ff2.pin(primitive::DFlipFlop::Terminal::NQ).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinNQ2.drive<Level>(l);
        });

        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CD4013B::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CD4013B::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CD4013B", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CD4013B: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CD4013B::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::Q1:
                return m_pinQ1;
            case Terminal::nQ1:
                return m_pinNQ1;
            case Terminal::CLOCK1:
                return m_pinCLOCK1;
            case Terminal::RESET1:
                return m_pinRESET1;
            case Terminal::D1:
                return m_pinD1;
            case Terminal::SET1:
                return m_pinSET1;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::SET2:
                return m_pinSET2;
            case Terminal::D2:
                return m_pinD2;
            case Terminal::RESET2:
                return m_pinRESET2;
            case Terminal::CLOCK2:
                return m_pinCLOCK2;
            case Terminal::nQ2:
                return m_pinNQ2;
            case Terminal::Q2:
                return m_pinQ2;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CD4013B", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CD4013B: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CD4013B::pins() const
    {
        return m_pinPtrs;
    }

    void CD4013B::reset()
    {
        m_ff1.reset();
        m_ff2.reset();
    }

    void CD4013B::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (!wasPowered && m_powered)
        {
            m_ff1.reset();
            m_ff2.reset();
        }
        else if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CD4013B",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            m_pinQ1.release();
            m_pinNQ1.release();
            m_pinQ2.release();
            m_pinNQ2.release();
        }
    }
} // namespace nfx::silicon::flipflop::cd4xxx
