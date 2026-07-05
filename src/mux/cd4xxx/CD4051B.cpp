#include "nfx/silicon/mux/cd4xxx/CD4051B.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::mux::cd4xxx
{
    namespace
    {
        using Kind = internal::runtime::error::Kind;

        signal::Pin makeAnalogIO(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Bidirectional } };
        }

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

    CD4051B::CD4051B(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinCH4{ makeAnalogIO("CH4") },
          m_pinCH6{ makeAnalogIO("CH6") },
          m_pinCOM{ makeAnalogIO("COM") },
          m_pinCH7{ makeAnalogIO("CH7") },
          m_pinCH5{ makeAnalogIO("CH5") },
          m_pinINH{ makeInput("INH") },
          m_pinVEE{ makePower("VEE") },
          m_pinVSS{ makePower("VSS") },
          m_pinC{ makeInput("C") },
          m_pinB{ makeInput("B") },
          m_pinA{ makeInput("A") },
          m_pinCH3{ makeAnalogIO("CH3") },
          m_pinCH0{ makeAnalogIO("CH0") },
          m_pinCH1{ makeAnalogIO("CH1") },
          m_pinCH2{ makeAnalogIO("CH2") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinCH4, &m_pinCH6, &m_pinCOM, &m_pinCH7, &m_pinCH5, &m_pinINH, &m_pinVEE, &m_pinVSS,
                     &m_pinC,   &m_pinB,   &m_pinA,   &m_pinCH3, &m_pinCH0, &m_pinCH1, &m_pinCH2, &m_pinVDD }
    {
        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CD4051B::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CD4051B::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CD4051B", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CD4051B: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CD4051B::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::CH4:
                return m_pinCH4;
            case Terminal::CH6:
                return m_pinCH6;
            case Terminal::COM:
                return m_pinCOM;
            case Terminal::CH7:
                return m_pinCH7;
            case Terminal::CH5:
                return m_pinCH5;
            case Terminal::INH:
                return m_pinINH;
            case Terminal::VEE:
                return m_pinVEE;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::C:
                return m_pinC;
            case Terminal::B:
                return m_pinB;
            case Terminal::A:
                return m_pinA;
            case Terminal::CH3:
                return m_pinCH3;
            case Terminal::CH0:
                return m_pinCH0;
            case Terminal::CH1:
                return m_pinCH1;
            case Terminal::CH2:
                return m_pinCH2;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CD4051B", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CD4051B: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CD4051B::pins() const
    {
        return m_pinPtrs;
    }

    std::optional<std::uint8_t> CD4051B::activeChannel() const
    {
        if (!m_powered)
        {
            return std::nullopt;
        }

        const Level inh = m_pinINH.read<Level>();
        const Level a = m_pinA.read<Level>();
        const Level b = m_pinB.read<Level>();
        const Level c = m_pinC.read<Level>();

        if (inh != Level::Low)
        {
            return std::nullopt;
        }
        if (a == Level::HighZ || b == Level::HighZ || c == Level::HighZ)
        {
            return std::nullopt;
        }

        const std::uint8_t sel =
            (c == Level::High ? 4u : 0u) | (b == Level::High ? 2u : 0u) | (a == Level::High ? 1u : 0u);
        return sel;
    }

    void CD4051B::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CD4051B",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: channel selection unavailable");
        }
    }
} // namespace nfx::silicon::mux::cd4xxx
