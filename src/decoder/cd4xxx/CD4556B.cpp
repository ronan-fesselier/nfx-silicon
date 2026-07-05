#include "nfx/silicon/decoder/cd4xxx/CD4556B.h"

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

    CD4556B::CD4556B(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinNE1{ makeInput("/E1") },
          m_pinA1{ makeInput("A1") },
          m_pinB1{ makeInput("B1") },
          m_pinNQ1_0{ makeOutput("/1Q0") },
          m_pinNQ1_1{ makeOutput("/1Q1") },
          m_pinNQ1_2{ makeOutput("/1Q2") },
          m_pinNQ1_3{ makeOutput("/1Q3") },
          m_pinVSS{ makePower("VSS") },
          m_pinNQ2_0{ makeOutput("/2Q0") },
          m_pinNQ2_1{ makeOutput("/2Q1") },
          m_pinNQ2_2{ makeOutput("/2Q2") },
          m_pinNQ2_3{ makeOutput("/2Q3") },
          m_pinB2{ makeInput("B2") },
          m_pinA2{ makeInput("A2") },
          m_pinNE2{ makeInput("/E2") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinNE1,   &m_pinA1,    &m_pinB1,    &m_pinNQ1_0, &m_pinNQ1_1, &m_pinNQ1_2, &m_pinNQ1_3, &m_pinVSS,
                     &m_pinNQ2_3, &m_pinNQ2_2, &m_pinNQ2_1, &m_pinNQ2_0, &m_pinB2,    &m_pinA2,    &m_pinNE2,   &m_pinVDD }
    {
        auto onDecoder1 = [this](const Level) { compute(0); };
        auto onDecoder2 = [this](const Level) { compute(1); };

        m_pinNE1.connect<Level>(onDecoder1);
        m_pinA1.connect<Level>(onDecoder1);
        m_pinB1.connect<Level>(onDecoder1);

        m_pinNE2.connect<Level>(onDecoder2);
        m_pinA2.connect<Level>(onDecoder2);
        m_pinB2.connect<Level>(onDecoder2);

        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CD4556B::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CD4556B::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CD4556B", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CD4556B: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CD4556B::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::nE1:
                return m_pinNE1;
            case Terminal::A1:
                return m_pinA1;
            case Terminal::B1:
                return m_pinB1;
            case Terminal::nQ1_0:
                return m_pinNQ1_0;
            case Terminal::nQ1_1:
                return m_pinNQ1_1;
            case Terminal::nQ1_2:
                return m_pinNQ1_2;
            case Terminal::nQ1_3:
                return m_pinNQ1_3;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::nQ2_3:
                return m_pinNQ2_3;
            case Terminal::nQ2_2:
                return m_pinNQ2_2;
            case Terminal::nQ2_1:
                return m_pinNQ2_1;
            case Terminal::nQ2_0:
                return m_pinNQ2_0;
            case Terminal::B2:
                return m_pinB2;
            case Terminal::A2:
                return m_pinA2;
            case Terminal::nE2:
                return m_pinNE2;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CD4556B", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CD4556B: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CD4556B::pins() const
    {
        return m_pinPtrs;
    }

    void CD4556B::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CD4556B",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            m_pinNQ1_0.release();
            m_pinNQ1_1.release();
            m_pinNQ1_2.release();
            m_pinNQ1_3.release();
            m_pinNQ2_0.release();
            m_pinNQ2_1.release();
            m_pinNQ2_2.release();
            m_pinNQ2_3.release();
        }
    }

    void CD4556B::compute(const std::uint8_t decoder)
    {
        if (!m_powered)
        {
            return;
        }

        signal::Pin& pinNE = decoder == 0 ? m_pinNE1 : m_pinNE2;
        signal::Pin& pinA  = decoder == 0 ? m_pinA1  : m_pinA2;
        signal::Pin& pinB  = decoder == 0 ? m_pinB1  : m_pinB2;
        signal::Pin* outs[4] = { decoder == 0 ? &m_pinNQ1_0 : &m_pinNQ2_0,
                                  decoder == 0 ? &m_pinNQ1_1 : &m_pinNQ2_1,
                                  decoder == 0 ? &m_pinNQ1_2 : &m_pinNQ2_2,
                                  decoder == 0 ? &m_pinNQ1_3 : &m_pinNQ2_3 };

        const Level ne = pinNE.read<Level>();
        const Level a  = pinA.read<Level>();
        const Level b  = pinB.read<Level>();

        if (ne == Level::HighZ || a == Level::HighZ || b == Level::HighZ)
        {
            for (auto* p : outs)
            {
                p->release();
            }
            return;
        }

        if (ne == Level::High)
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
} // namespace nfx::silicon::decoder::cd4xxx
