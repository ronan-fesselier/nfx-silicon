#include "nfx/silicon/decoder/cd4xxx/CD4028B.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::decoder::cd4xxx
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

    CD4028B::CD4028B(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinY4{ makeOutput("Y4") },
          m_pinY2{ makeOutput("Y2") },
          m_pinY0{ makeOutput("Y0") },
          m_pinY7{ makeOutput("Y7") },
          m_pinY9{ makeOutput("Y9") },
          m_pinY5{ makeOutput("Y5") },
          m_pinY6{ makeOutput("Y6") },
          m_pinVSS{ makePower("VSS") },
          m_pinY8{ makeOutput("Y8") },
          m_pinA{ makeInput("A") },
          m_pinD{ makeInput("D") },
          m_pinC{ makeInput("C") },
          m_pinB{ makeInput("B") },
          m_pinY1{ makeOutput("Y1") },
          m_pinY3{ makeOutput("Y3") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinY4, &m_pinY2, &m_pinY0, &m_pinY7, &m_pinY9, &m_pinY5, &m_pinY6, &m_pinVSS,
                     &m_pinY8, &m_pinA,  &m_pinD,  &m_pinC,  &m_pinB,  &m_pinY1, &m_pinY3, &m_pinVDD },
          m_yPtrs{ &m_pinY0, &m_pinY1, &m_pinY2, &m_pinY3, &m_pinY4, &m_pinY5, &m_pinY6, &m_pinY7, &m_pinY8, &m_pinY9 }
    {
        auto onChange = [this](const Level) { compute(); };

        m_pinA.connect<Level>(onChange);
        m_pinB.connect<Level>(onChange);
        m_pinC.connect<Level>(onChange);
        m_pinD.connect<Level>(onChange);

        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CD4028B::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CD4028B::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CD4028B", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CD4028B: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CD4028B::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::Y4:
                return m_pinY4;
            case Terminal::Y2:
                return m_pinY2;
            case Terminal::Y0:
                return m_pinY0;
            case Terminal::Y7:
                return m_pinY7;
            case Terminal::Y9:
                return m_pinY9;
            case Terminal::Y5:
                return m_pinY5;
            case Terminal::Y6:
                return m_pinY6;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::Y8:
                return m_pinY8;
            case Terminal::A:
                return m_pinA;
            case Terminal::D:
                return m_pinD;
            case Terminal::C:
                return m_pinC;
            case Terminal::B:
                return m_pinB;
            case Terminal::Y1:
                return m_pinY1;
            case Terminal::Y3:
                return m_pinY3;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CD4028B", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CD4028B: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CD4028B::pins() const
    {
        return m_pinPtrs;
    }

    void CD4028B::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CD4028B",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            for (auto* p : m_yPtrs)
            {
                p->release();
            }
        }
    }

    void CD4028B::compute()
    {
        if (!m_powered)
        {
            return;
        }

        const Level a = m_pinA.read<Level>();
        const Level b = m_pinB.read<Level>();
        const Level c = m_pinC.read<Level>();
        const Level d = m_pinD.read<Level>();

        if (a == Level::HighZ || b == Level::HighZ || c == Level::HighZ || d == Level::HighZ)
        {
            for (auto* p : m_yPtrs)
            {
                p->release();
            }
            return;
        }

        const std::uint8_t code = (d == Level::High ? 8u : 0u) | (c == Level::High ? 4u : 0u) |
                                  (b == Level::High ? 2u : 0u) | (a == Level::High ? 1u : 0u);

        for (std::uint8_t i = 0; i < 10; ++i)
        {
            m_yPtrs[i]->drive<Level>(i == code ? Level::High : Level::Low);
        }
    }
} // namespace nfx::silicon::decoder::cd4xxx
