#include "nfx/silicon/gate/sn74xxx/LS86.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::gate::sn74xxx
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
    } // namespace

    LS86::LS86(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_gates{ primitive::Xor{ primitive::Xor::Descriptor{ .name = "XOR1" } },
                   primitive::Xor{ primitive::Xor::Descriptor{ .name = "XOR2" } },
                   primitive::Xor{ primitive::Xor::Descriptor{ .name = "XOR3" } },
                   primitive::Xor{ primitive::Xor::Descriptor{ .name = "XOR4" } } },
          m_pinA1{ makeInput("1A") },
          m_pinB1{ makeInput("1B") },
          m_pinY1{ makeOutput("1Y") },
          m_pinA2{ makeInput("2A") },
          m_pinB2{ makeInput("2B") },
          m_pinY2{ makeOutput("2Y") },
          m_pinGND{ makePower("GND") },
          m_pinY3{ makeOutput("3Y") },
          m_pinA3{ makeInput("3A") },
          m_pinB3{ makeInput("3B") },
          m_pinY4{ makeOutput("4Y") },
          m_pinA4{ makeInput("4A") },
          m_pinB4{ makeInput("4B") },
          m_pinVCC{ makePower("VCC") },
          m_pinPtrs{ &m_pinA1, &m_pinB1, &m_pinY1, &m_pinA2, &m_pinB2, &m_pinY2, &m_pinGND,
                     &m_pinY3, &m_pinA3, &m_pinB3, &m_pinY4, &m_pinA4, &m_pinB4, &m_pinVCC }
    {
        auto wireGate = [this](const std::size_t idx, signal::Pin& pinA, signal::Pin& pinB, signal::Pin& pinY) {
            pinA.connect<Level>([this, idx](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                m_gates[idx].pin(primitive::Xor::Terminal::A).drive<Level>(l);
            });

            pinB.connect<Level>([this, idx](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                m_gates[idx].pin(primitive::Xor::Terminal::B).drive<Level>(l);
            });

            m_gates[idx].pin(primitive::Xor::Terminal::Y).connect<Level>([this, &pinY](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                pinY.drive<Level>(l);
            });
        };

        wireGate(0, m_pinA1, m_pinB1, m_pinY1);
        wireGate(1, m_pinA2, m_pinB2, m_pinY2);
        wireGate(2, m_pinA3, m_pinB3, m_pinY3);
        wireGate(3, m_pinA4, m_pinB4, m_pinY4);

        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* LS86::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LS86::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LS86", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "LS86: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LS86::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A1:
                return m_pinA1;
            case Terminal::B1:
                return m_pinB1;
            case Terminal::Y1:
                return m_pinY1;
            case Terminal::A2:
                return m_pinA2;
            case Terminal::B2:
                return m_pinB2;
            case Terminal::Y2:
                return m_pinY2;
            case Terminal::GND:
                return m_pinGND;
            case Terminal::Y3:
                return m_pinY3;
            case Terminal::A3:
                return m_pinA3;
            case Terminal::B3:
                return m_pinB3;
            case Terminal::Y4:
                return m_pinY4;
            case Terminal::A4:
                return m_pinA4;
            case Terminal::B4:
                return m_pinB4;
            case Terminal::VCC:
                return m_pinVCC;
        }
        internal::runtime::error::log(
            "LS86", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "LS86: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> LS86::pins() const
    {
        return m_pinPtrs;
    }

    void LS86::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "LS86",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: outputs held at HighZ");
            m_pinY1.release();
            m_pinY2.release();
            m_pinY3.release();
            m_pinY4.release();
        }
    }
} // namespace nfx::silicon::gate::sn74xxx
