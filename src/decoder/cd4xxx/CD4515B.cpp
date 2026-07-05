#include "nfx/silicon/decoder/cd4xxx/CD4515B.h"

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

    CD4515B::CD4515B(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinSTROBE{ makeInput("STROBE") },
          m_pinDATA1{ makeInput("DATA1") },
          m_pinDATA2{ makeInput("DATA2") },
          m_pinS7{ makeOutput("S7") },
          m_pinS6{ makeOutput("S6") },
          m_pinS5{ makeOutput("S5") },
          m_pinS4{ makeOutput("S4") },
          m_pinS3{ makeOutput("S3") },
          m_pinS1{ makeOutput("S1") },
          m_pinS2{ makeOutput("S2") },
          m_pinS0{ makeOutput("S0") },
          m_pinVSS{ makePower("VSS") },
          m_pinS13{ makeOutput("S13") },
          m_pinS12{ makeOutput("S12") },
          m_pinS15{ makeOutput("S15") },
          m_pinS14{ makeOutput("S14") },
          m_pinS9{ makeOutput("S9") },
          m_pinS8{ makeOutput("S8") },
          m_pinS11{ makeOutput("S11") },
          m_pinS10{ makeOutput("S10") },
          m_pinDATA3{ makeInput("DATA3") },
          m_pinDATA4{ makeInput("DATA4") },
          m_pinINHIBIT{ makeInput("INHIBIT") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinSTROBE, &m_pinDATA1, &m_pinDATA2, &m_pinS7,    &m_pinS6,      &m_pinS5,
                     &m_pinS4,     &m_pinS3,    &m_pinS1,    &m_pinS2,    &m_pinS0,      &m_pinVSS,
                     &m_pinS13,    &m_pinS12,   &m_pinS15,   &m_pinS14,   &m_pinS9,      &m_pinS8,
                     &m_pinS11,    &m_pinS10,   &m_pinDATA3, &m_pinDATA4, &m_pinINHIBIT, &m_pinVDD },
          m_sPtrs{ &m_pinS0, &m_pinS1, &m_pinS2,  &m_pinS3,  &m_pinS4,  &m_pinS5,  &m_pinS6,  &m_pinS7,
                   &m_pinS8, &m_pinS9, &m_pinS10, &m_pinS11, &m_pinS12, &m_pinS13, &m_pinS14, &m_pinS15 }
    {
        auto onChange = [this](const Level) { compute(); };

        m_pinDATA1.connect<Level>(onChange);
        m_pinDATA2.connect<Level>(onChange);
        m_pinDATA3.connect<Level>(onChange);
        m_pinDATA4.connect<Level>(onChange);
        m_pinSTROBE.connect<Level>(onChange);
        m_pinINHIBIT.connect<Level>(onChange);

        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CD4515B::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CD4515B::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CD4515B", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CD4515B: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CD4515B::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::STROBE:
                return m_pinSTROBE;
            case Terminal::DATA1:
                return m_pinDATA1;
            case Terminal::DATA2:
                return m_pinDATA2;
            case Terminal::S7:
                return m_pinS7;
            case Terminal::S6:
                return m_pinS6;
            case Terminal::S5:
                return m_pinS5;
            case Terminal::S4:
                return m_pinS4;
            case Terminal::S3:
                return m_pinS3;
            case Terminal::S1:
                return m_pinS1;
            case Terminal::S2:
                return m_pinS2;
            case Terminal::S0:
                return m_pinS0;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::S13:
                return m_pinS13;
            case Terminal::S12:
                return m_pinS12;
            case Terminal::S15:
                return m_pinS15;
            case Terminal::S14:
                return m_pinS14;
            case Terminal::S9:
                return m_pinS9;
            case Terminal::S8:
                return m_pinS8;
            case Terminal::S11:
                return m_pinS11;
            case Terminal::S10:
                return m_pinS10;
            case Terminal::DATA3:
                return m_pinDATA3;
            case Terminal::DATA4:
                return m_pinDATA4;
            case Terminal::INHIBIT:
                return m_pinINHIBIT;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CD4515B", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CD4515B: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CD4515B::pins() const
    {
        return m_pinPtrs;
    }

    void CD4515B::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CD4515B",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            for (auto* p : m_sPtrs)
            {
                p->release();
            }
        }
    }

    void CD4515B::compute()
    {
        if (!m_powered)
        {
            return;
        }

        const Level strobe = m_pinSTROBE.read<Level>();
        const Level inhibit = m_pinINHIBIT.read<Level>();

        if (strobe == Level::Low)
        {
            // latched: outputs hold, update latch silently
            const Level a = m_pinDATA1.read<Level>();
            const Level b = m_pinDATA2.read<Level>();
            const Level c = m_pinDATA3.read<Level>();
            const Level d = m_pinDATA4.read<Level>();
            if (a != Level::HighZ && b != Level::HighZ && c != Level::HighZ && d != Level::HighZ)
            {
                m_latch = (d == Level::High ? 8u : 0u) | (c == Level::High ? 4u : 0u) | (b == Level::High ? 2u : 0u) |
                          (a == Level::High ? 1u : 0u);
            }
            return;
        }

        if (inhibit == Level::High)
        {
            for (auto* p : m_sPtrs)
            {
                p->drive<Level>(Level::High);
            }
            return;
        }

        const Level a = m_pinDATA1.read<Level>();
        const Level b = m_pinDATA2.read<Level>();
        const Level c = m_pinDATA3.read<Level>();
        const Level d = m_pinDATA4.read<Level>();

        if (a == Level::HighZ || b == Level::HighZ || c == Level::HighZ || d == Level::HighZ)
        {
            for (auto* p : m_sPtrs)
            {
                p->release();
            }
            return;
        }

        m_latch = (d == Level::High ? 8u : 0u) | (c == Level::High ? 4u : 0u) | (b == Level::High ? 2u : 0u) |
                  (a == Level::High ? 1u : 0u);

        for (std::uint8_t i = 0; i < 16; ++i)
        {
            m_sPtrs[i]->drive<Level>(i == m_latch ? Level::Low : Level::High);
        }
    }
} // namespace nfx::silicon::decoder::cd4xxx
