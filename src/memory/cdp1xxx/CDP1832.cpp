#include "nfx/silicon/memory/cdp1xxx/CDP1832.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::memory::cdp1xxx
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

        signal::Pin makeNC(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } };
        }
    } // namespace

    CDP1832::CDP1832(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinMA7{ makeInput("MA7") },
          m_pinMA6{ makeInput("MA6") },
          m_pinMA5{ makeInput("MA5") },
          m_pinMA4{ makeInput("MA4") },
          m_pinMA3{ makeInput("MA3") },
          m_pinMA2{ makeInput("MA2") },
          m_pinMA1{ makeInput("MA1") },
          m_pinMA0{ makeInput("MA0") },
          m_pinBUS0{ makeOutput("BUS0") },
          m_pinBUS1{ makeOutput("BUS1") },
          m_pinBUS2{ makeOutput("BUS2") },
          m_pinVSS{ makePower("VSS") },
          m_pinBUS3{ makeOutput("BUS3") },
          m_pinBUS4{ makeOutput("BUS4") },
          m_pinBUS5{ makeOutput("BUS5") },
          m_pinBUS6{ makeOutput("BUS6") },
          m_pinBUS7{ makeOutput("BUS7") },
          m_pinNC18{ makeNC("NC18") },
          m_pinNC19{ makeNC("NC19") },
          m_pinCS{ makeInput("CS") },
          m_pinNC21{ makeNC("NC21") },
          m_pinNC22{ makeNC("NC22") },
          m_pinA8{ makeInput("A8") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinMA7,  &m_pinMA6,  &m_pinMA5,  &m_pinMA4, &m_pinMA3,  &m_pinMA2,  &m_pinMA1,  &m_pinMA0,
                     &m_pinBUS0, &m_pinBUS1, &m_pinBUS2, &m_pinVSS, &m_pinBUS3, &m_pinBUS4, &m_pinBUS5, &m_pinBUS6,
                     &m_pinBUS7, &m_pinNC18, &m_pinNC19, &m_pinCS,  &m_pinNC21, &m_pinNC22, &m_pinA8,   &m_pinVDD },
          m_maPtrs{ &m_pinMA0, &m_pinMA1, &m_pinMA2, &m_pinMA3, &m_pinMA4, &m_pinMA5, &m_pinMA6, &m_pinMA7, &m_pinA8 },
          m_busPtrs{ &m_pinBUS0, &m_pinBUS1, &m_pinBUS2, &m_pinBUS3, &m_pinBUS4, &m_pinBUS5, &m_pinBUS6, &m_pinBUS7 }
    {
        auto onChange = [this](const Level) { compute(); };

        m_pinMA0.connect<Level>(onChange);
        m_pinMA1.connect<Level>(onChange);
        m_pinMA2.connect<Level>(onChange);
        m_pinMA3.connect<Level>(onChange);
        m_pinMA4.connect<Level>(onChange);
        m_pinMA5.connect<Level>(onChange);
        m_pinMA6.connect<Level>(onChange);
        m_pinMA7.connect<Level>(onChange);
        m_pinA8.connect<Level>(onChange);
        m_pinCS.connect<Level>(onChange);

        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CDP1832::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CDP1832::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CDP1832", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CDP1832: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CDP1832::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::MA7:
                return m_pinMA7;
            case Terminal::MA6:
                return m_pinMA6;
            case Terminal::MA5:
                return m_pinMA5;
            case Terminal::MA4:
                return m_pinMA4;
            case Terminal::MA3:
                return m_pinMA3;
            case Terminal::MA2:
                return m_pinMA2;
            case Terminal::MA1:
                return m_pinMA1;
            case Terminal::MA0:
                return m_pinMA0;
            case Terminal::BUS0:
                return m_pinBUS0;
            case Terminal::BUS1:
                return m_pinBUS1;
            case Terminal::BUS2:
                return m_pinBUS2;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::BUS3:
                return m_pinBUS3;
            case Terminal::BUS4:
                return m_pinBUS4;
            case Terminal::BUS5:
                return m_pinBUS5;
            case Terminal::BUS6:
                return m_pinBUS6;
            case Terminal::BUS7:
                return m_pinBUS7;
            case Terminal::NC18:
                return m_pinNC18;
            case Terminal::NC19:
                return m_pinNC19;
            case Terminal::CS:
                return m_pinCS;
            case Terminal::NC21:
                return m_pinNC21;
            case Terminal::NC22:
                return m_pinNC22;
            case Terminal::A8:
                return m_pinA8;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CDP1832", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CDP1832: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CDP1832::pins() const
    {
        return m_pinPtrs;
    }

    void CDP1832::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CDP1832",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            for (auto* p : m_busPtrs)
            {
                p->release();
            }
        }
        else if (!wasPowered && m_powered)
        {
            compute();
        }
    }

    void CDP1832::compute()
    {
        if (!m_powered)
        {
            return;
        }

        const Level cs = m_pinCS.read<Level>();

        if (cs != Level::Low)
        {
            for (auto* p : m_busPtrs)
            {
                p->release();
            }
            return;
        }

        std::uint16_t addr = 0;
        for (std::uint8_t i = 0; i < 9; ++i)
        {
            const Level bit = m_maPtrs[i]->read<Level>();
            if (bit == Level::HighZ)
            {
                for (auto* p : m_busPtrs)
                {
                    p->release();
                }
                return;
            }
            if (bit == Level::High)
            {
                addr |= static_cast<std::uint16_t>(1u << i);
            }
        }

        const std::uint8_t byte = m_descriptor.rom[addr];
        for (std::uint8_t i = 0; i < 8; ++i)
        {
            m_busPtrs[i]->drive<Level>((byte >> i) & 1u ? Level::High : Level::Low);
        }
    }
} // namespace nfx::silicon::memory::cdp1xxx
