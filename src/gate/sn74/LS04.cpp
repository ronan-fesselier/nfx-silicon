#include "nfx/silicon/gate/sn74xxx/LS04.h"

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

    LS04::LS04(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_inverters{ Not{ Not::Descriptor{ .name = "INV1" } }, Not{ Not::Descriptor{ .name = "INV2" } },
                       Not{ Not::Descriptor{ .name = "INV3" } }, Not{ Not::Descriptor{ .name = "INV4" } },
                       Not{ Not::Descriptor{ .name = "INV5" } }, Not{ Not::Descriptor{ .name = "INV6" } } },
          m_pinA1{ makeInput("1A") },
          m_pinY1{ makeOutput("1Y") },
          m_pinA2{ makeInput("2A") },
          m_pinY2{ makeOutput("2Y") },
          m_pinA3{ makeInput("3A") },
          m_pinY3{ makeOutput("3Y") },
          m_pinGND{ makePower("GND") },
          m_pinY4{ makeOutput("4Y") },
          m_pinA4{ makeInput("4A") },
          m_pinY5{ makeOutput("5Y") },
          m_pinA5{ makeInput("5A") },
          m_pinY6{ makeOutput("6Y") },
          m_pinA6{ makeInput("6A") },
          m_pinVCC{ makePower("VCC") },
          m_pinPtrs{ &m_pinA1, &m_pinY1, &m_pinA2, &m_pinY2, &m_pinA3, &m_pinY3, &m_pinGND,
                     &m_pinY4, &m_pinA4, &m_pinY5, &m_pinA5, &m_pinY6, &m_pinA6, &m_pinVCC }
    {
        auto wireInverter = [this](const std::size_t idx, signal::Pin& pinA, signal::Pin& pinY) {
            pinA.connect<Level>([this, idx](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                m_inverters[idx].pin(Not::Terminal::A).drive<Level>(l);
            });

            m_inverters[idx].pin(Not::Terminal::Y).connect<Level>([this, &pinY](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                pinY.drive<Level>(l);
            });
        };

        wireInverter(0, m_pinA1, m_pinY1);
        wireInverter(1, m_pinA2, m_pinY2);
        wireInverter(2, m_pinA3, m_pinY3);
        wireInverter(3, m_pinA4, m_pinY4);
        wireInverter(4, m_pinA5, m_pinY5);
        wireInverter(5, m_pinA6, m_pinY6);

        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* LS04::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LS04::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LS04", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "LS04: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LS04::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A1:
                return m_pinA1;
            case Terminal::Y1:
                return m_pinY1;
            case Terminal::A2:
                return m_pinA2;
            case Terminal::Y2:
                return m_pinY2;
            case Terminal::A3:
                return m_pinA3;
            case Terminal::Y3:
                return m_pinY3;
            case Terminal::GND:
                return m_pinGND;
            case Terminal::Y4:
                return m_pinY4;
            case Terminal::A4:
                return m_pinA4;
            case Terminal::Y5:
                return m_pinY5;
            case Terminal::A5:
                return m_pinA5;
            case Terminal::Y6:
                return m_pinY6;
            case Terminal::A6:
                return m_pinA6;
            case Terminal::VCC:
                return m_pinVCC;
        }
        internal::runtime::error::log(
            "LS04", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "LS04: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> LS04::pins() const
    {
        return m_pinPtrs;
    }

    void LS04::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "LS04",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: outputs held at HighZ");
            m_pinY1.release();
            m_pinY2.release();
            m_pinY3.release();
            m_pinY4.release();
            m_pinY5.release();
            m_pinY6.release();
        }
    }
} // namespace nfx::silicon::gate::sn74
