#include "nfx/silicon/gate/sn74xxx/LS138.h"

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

    LS138::LS138(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinA{ makeInput("A") },
          m_pinB{ makeInput("B") },
          m_pinC{ makeInput("C") },
          m_pinNG2A{ makeInput("/G2A") },
          m_pinNG2B{ makeInput("/G2B") },
          m_pinG1{ makeInput("G1") },
          m_pinY7{ makeOutput("Y7") },
          m_pinGND{ makePower("GND") },
          m_pinY6{ makeOutput("Y6") },
          m_pinY5{ makeOutput("Y5") },
          m_pinY4{ makeOutput("Y4") },
          m_pinY3{ makeOutput("Y3") },
          m_pinY2{ makeOutput("Y2") },
          m_pinY1{ makeOutput("Y1") },
          m_pinY0{ makeOutput("Y0") },
          m_pinVCC{ makePower("VCC") },
          m_pinPtrs{ &m_pinA,  &m_pinB,  &m_pinC,  &m_pinNG2A, &m_pinNG2B, &m_pinG1, &m_pinY7, &m_pinGND,
                     &m_pinY6, &m_pinY5, &m_pinY4, &m_pinY3,   &m_pinY2,   &m_pinY1, &m_pinY0, &m_pinVCC }
    {
        auto onInput = [this](const Level) { compute(); };

        m_pinA.connect<Level>(onInput);
        m_pinB.connect<Level>(onInput);
        m_pinC.connect<Level>(onInput);
        m_pinNG2A.connect<Level>(onInput);
        m_pinNG2B.connect<Level>(onInput);
        m_pinG1.connect<Level>(onInput);

        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* LS138::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LS138::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LS138", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "LS138: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LS138::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinA;
            case Terminal::B:
                return m_pinB;
            case Terminal::C:
                return m_pinC;
            case Terminal::nG2A:
                return m_pinNG2A;
            case Terminal::nG2B:
                return m_pinNG2B;
            case Terminal::G1:
                return m_pinG1;
            case Terminal::Y7:
                return m_pinY7;
            case Terminal::GND:
                return m_pinGND;
            case Terminal::Y6:
                return m_pinY6;
            case Terminal::Y5:
                return m_pinY5;
            case Terminal::Y4:
                return m_pinY4;
            case Terminal::Y3:
                return m_pinY3;
            case Terminal::Y2:
                return m_pinY2;
            case Terminal::Y1:
                return m_pinY1;
            case Terminal::Y0:
                return m_pinY0;
            case Terminal::VCC:
                return m_pinVCC;
        }
        internal::runtime::error::log(
            "LS138", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "LS138: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> LS138::pins() const
    {
        return m_pinPtrs;
    }

    void LS138::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "LS138",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: outputs held at HighZ");
            m_pinY0.release();
            m_pinY1.release();
            m_pinY2.release();
            m_pinY3.release();
            m_pinY4.release();
            m_pinY5.release();
            m_pinY6.release();
            m_pinY7.release();
        }
    }

    void LS138::compute()
    {
        if (!m_powered)
        {
            return;
        }

        const Level a = m_pinA.read<Level>();
        const Level b = m_pinB.read<Level>();
        const Level c = m_pinC.read<Level>();
        const Level g1 = m_pinG1.read<Level>();
        const Level g2a = m_pinNG2A.read<Level>();
        const Level g2b = m_pinNG2B.read<Level>();

        // Any HighZ input -> all outputs HighZ
        if (a == Level::HighZ || b == Level::HighZ || c == Level::HighZ || g1 == Level::HighZ || g2a == Level::HighZ ||
            g2b == Level::HighZ)
        {
            m_pinY0.release();
            m_pinY1.release();
            m_pinY2.release();
            m_pinY3.release();
            m_pinY4.release();
            m_pinY5.release();
            m_pinY6.release();
            m_pinY7.release();
            return;
        }

        // Enable: G1=High, /G2A=Low, /G2B=Low
        const bool enabled = (g1 == Level::High) && (g2a == Level::Low) && (g2b == Level::Low);

        if (!enabled)
        {
            m_pinY0.drive<Level>(Level::High);
            m_pinY1.drive<Level>(Level::High);
            m_pinY2.drive<Level>(Level::High);
            m_pinY3.drive<Level>(Level::High);
            m_pinY4.drive<Level>(Level::High);
            m_pinY5.drive<Level>(Level::High);
            m_pinY6.drive<Level>(Level::High);
            m_pinY7.drive<Level>(Level::High);
            return;
        }

        const std::uint8_t addr =
            (c == Level::High ? 4u : 0u) | (b == Level::High ? 2u : 0u) | (a == Level::High ? 1u : 0u);

        signal::Pin* outputs[8] = { &m_pinY0, &m_pinY1, &m_pinY2, &m_pinY3, &m_pinY4, &m_pinY5, &m_pinY6, &m_pinY7 };

        for (std::uint8_t i = 0; i < 8; ++i)
        {
            outputs[i]->drive<Level>(i == addr ? Level::Low : Level::High);
        }
    }

} // namespace nfx::silicon::gate::sn74
