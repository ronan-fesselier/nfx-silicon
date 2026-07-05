#include "nfx/silicon/decoder/sn74xxx/LS139.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::decoder::sn74xxx
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

    LS139::LS139(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinNG1{ makeInput("/1G") },
          m_pinA1{ makeInput("1A") },
          m_pinB1{ makeInput("1B") },
          m_pinY1_0{ makeOutput("1Y0") },
          m_pinY1_1{ makeOutput("1Y1") },
          m_pinY1_2{ makeOutput("1Y2") },
          m_pinY1_3{ makeOutput("1Y3") },
          m_pinGND{ makePower("GND") },
          m_pinY2_3{ makeOutput("2Y3") },
          m_pinY2_2{ makeOutput("2Y2") },
          m_pinY2_1{ makeOutput("2Y1") },
          m_pinY2_0{ makeOutput("2Y0") },
          m_pinB2{ makeInput("2B") },
          m_pinA2{ makeInput("2A") },
          m_pinNG2{ makeInput("/2G") },
          m_pinVCC{ makePower("VCC") },
          m_pinPtrs{ &m_pinNG1,  &m_pinA1,   &m_pinB1,   &m_pinY1_0, &m_pinY1_1, &m_pinY1_2, &m_pinY1_3, &m_pinGND,
                     &m_pinY2_3, &m_pinY2_2, &m_pinY2_1, &m_pinY2_0, &m_pinB2,   &m_pinA2,   &m_pinNG2,  &m_pinVCC }
    {
        auto onDecoder1 = [this](const Level) { compute(0); };
        auto onDecoder2 = [this](const Level) { compute(1); };

        m_pinNG1.connect<Level>(onDecoder1);
        m_pinA1.connect<Level>(onDecoder1);
        m_pinB1.connect<Level>(onDecoder1);

        m_pinNG2.connect<Level>(onDecoder2);
        m_pinA2.connect<Level>(onDecoder2);
        m_pinB2.connect<Level>(onDecoder2);

        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* LS139::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LS139::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LS139", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "LS139: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LS139::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::nG1:
                return m_pinNG1;
            case Terminal::A1:
                return m_pinA1;
            case Terminal::B1:
                return m_pinB1;
            case Terminal::Y1_0:
                return m_pinY1_0;
            case Terminal::Y1_1:
                return m_pinY1_1;
            case Terminal::Y1_2:
                return m_pinY1_2;
            case Terminal::Y1_3:
                return m_pinY1_3;
            case Terminal::GND:
                return m_pinGND;
            case Terminal::Y2_3:
                return m_pinY2_3;
            case Terminal::Y2_2:
                return m_pinY2_2;
            case Terminal::Y2_1:
                return m_pinY2_1;
            case Terminal::Y2_0:
                return m_pinY2_0;
            case Terminal::B2:
                return m_pinB2;
            case Terminal::A2:
                return m_pinA2;
            case Terminal::nG2:
                return m_pinNG2;
            case Terminal::VCC:
                return m_pinVCC;
        }
        internal::runtime::error::log(
            "LS139", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "LS139: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> LS139::pins() const
    {
        return m_pinPtrs;
    }

    void LS139::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "LS139",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: outputs held at HighZ");
            m_pinY1_0.release();
            m_pinY1_1.release();
            m_pinY1_2.release();
            m_pinY1_3.release();
            m_pinY2_0.release();
            m_pinY2_1.release();
            m_pinY2_2.release();
            m_pinY2_3.release();
        }
    }

    void LS139::compute(const std::uint8_t decoder)
    {
        if (!m_powered)
        {
            return;
        }

        signal::Pin& pinG = decoder == 0 ? m_pinNG1 : m_pinNG2;
        signal::Pin& pinA = decoder == 0 ? m_pinA1 : m_pinA2;
        signal::Pin& pinB = decoder == 0 ? m_pinB1 : m_pinB2;
        signal::Pin* outs[4] = { decoder == 0 ? &m_pinY1_0 : &m_pinY2_0,
                                 decoder == 0 ? &m_pinY1_1 : &m_pinY2_1,
                                 decoder == 0 ? &m_pinY1_2 : &m_pinY2_2,
                                 decoder == 0 ? &m_pinY1_3 : &m_pinY2_3 };

        const Level g = pinG.read<Level>();
        const Level a = pinA.read<Level>();
        const Level b = pinB.read<Level>();

        if (g == Level::HighZ || a == Level::HighZ || b == Level::HighZ)
        {
            for (auto* p : outs)
            {
                p->release();
            }
            return;
        }

        if (g == Level::High)
        {
            for (auto* p : outs)
            {
                p->drive<Level>(Level::High);
            }
            return;
        }

        const std::uint8_t addr = (b == Level::High ? 2u : 0u) | (a == Level::High ? 1u : 0u);

        for (std::uint8_t i = 0; i < 4; ++i)
        {
            outs[i]->drive<Level>(i == addr ? Level::Low : Level::High);
        }
    }
} // namespace nfx::silicon::decoder::sn74xxx
