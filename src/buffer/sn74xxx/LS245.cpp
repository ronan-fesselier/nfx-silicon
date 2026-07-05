#include "nfx/silicon/buffer/sn74xxx/LS245.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::buffer::sn74xxx
{
    namespace
    {
        using Kind = internal::runtime::error::Kind;

        signal::Pin makeBidir(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{ .name = name,
                                                         .kind = signal::Pin::Kind::Digital,
                                                         .direction = signal::Pin::Direction::Bidirectional } };
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

    LS245::LS245(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinDIR{ makeInput("DIR") },
          m_pinA1{ makeBidir("A1") },
          m_pinA2{ makeBidir("A2") },
          m_pinA3{ makeBidir("A3") },
          m_pinA4{ makeBidir("A4") },
          m_pinA5{ makeBidir("A5") },
          m_pinA6{ makeBidir("A6") },
          m_pinA7{ makeBidir("A7") },
          m_pinA8{ makeBidir("A8") },
          m_pinGND{ makePower("GND") },
          m_pinB8{ makeBidir("B8") },
          m_pinB7{ makeBidir("B7") },
          m_pinB6{ makeBidir("B6") },
          m_pinB5{ makeBidir("B5") },
          m_pinB4{ makeBidir("B4") },
          m_pinB3{ makeBidir("B3") },
          m_pinB2{ makeBidir("B2") },
          m_pinB1{ makeBidir("B1") },
          m_pinNOE{ makeInput("/OE") },
          m_pinVCC{ makePower("VCC") },
          m_pinPtrs{ &m_pinDIR, &m_pinA1, &m_pinA2,  &m_pinA3, &m_pinA4,  &m_pinA5, &m_pinA6,
                     &m_pinA7,  &m_pinA8, &m_pinGND, &m_pinB8, &m_pinB7,  &m_pinB6, &m_pinB5,
                     &m_pinB4,  &m_pinB3, &m_pinB2,  &m_pinB1, &m_pinNOE, &m_pinVCC }
    {
        auto onInput = [this](const Level) { compute(); };

        m_pinDIR.connect<Level>(onInput);
        m_pinNOE.connect<Level>(onInput);
        m_pinA1.connect<Level>(onInput);
        m_pinA2.connect<Level>(onInput);
        m_pinA3.connect<Level>(onInput);
        m_pinA4.connect<Level>(onInput);
        m_pinA5.connect<Level>(onInput);
        m_pinA6.connect<Level>(onInput);
        m_pinA7.connect<Level>(onInput);
        m_pinA8.connect<Level>(onInput);
        m_pinB1.connect<Level>(onInput);
        m_pinB2.connect<Level>(onInput);
        m_pinB3.connect<Level>(onInput);
        m_pinB4.connect<Level>(onInput);
        m_pinB5.connect<Level>(onInput);
        m_pinB6.connect<Level>(onInput);
        m_pinB7.connect<Level>(onInput);
        m_pinB8.connect<Level>(onInput);

        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* LS245::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LS245::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LS245", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "LS245: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LS245::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::DIR:
                return m_pinDIR;
            case Terminal::A1:
                return m_pinA1;
            case Terminal::A2:
                return m_pinA2;
            case Terminal::A3:
                return m_pinA3;
            case Terminal::A4:
                return m_pinA4;
            case Terminal::A5:
                return m_pinA5;
            case Terminal::A6:
                return m_pinA6;
            case Terminal::A7:
                return m_pinA7;
            case Terminal::A8:
                return m_pinA8;
            case Terminal::GND:
                return m_pinGND;
            case Terminal::B8:
                return m_pinB8;
            case Terminal::B7:
                return m_pinB7;
            case Terminal::B6:
                return m_pinB6;
            case Terminal::B5:
                return m_pinB5;
            case Terminal::B4:
                return m_pinB4;
            case Terminal::B3:
                return m_pinB3;
            case Terminal::B2:
                return m_pinB2;
            case Terminal::B1:
                return m_pinB1;
            case Terminal::nOE:
                return m_pinNOE;
            case Terminal::VCC:
                return m_pinVCC;
        }
        internal::runtime::error::log(
            "LS245", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "LS245: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> LS245::pins() const
    {
        return m_pinPtrs;
    }

    void LS245::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "LS245",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: outputs held at HighZ");
            releaseAll();
        }
    }

    void LS245::releaseAll()
    {
        m_pinA1.release();
        m_pinA2.release();
        m_pinA3.release();
        m_pinA4.release();
        m_pinA5.release();
        m_pinA6.release();
        m_pinA7.release();
        m_pinA8.release();
        m_pinB1.release();
        m_pinB2.release();
        m_pinB3.release();
        m_pinB4.release();
        m_pinB5.release();
        m_pinB6.release();
        m_pinB7.release();
        m_pinB8.release();
    }

    void LS245::compute()
    {
        if (!m_powered || m_computing)
            return;
        m_computing = true;

        const Level oe = m_pinNOE.read<Level>();
        const Level dir = m_pinDIR.read<Level>();

        if (oe == Level::HighZ || dir == Level::HighZ || oe == Level::High)
        {
            releaseAll();
            m_lastDir = Level::HighZ;
            m_computing = false;
            return;
        }

        signal::Pin* a[8] = { &m_pinA1, &m_pinA2, &m_pinA3, &m_pinA4, &m_pinA5, &m_pinA6, &m_pinA7, &m_pinA8 };
        signal::Pin* b[8] = { &m_pinB1, &m_pinB2, &m_pinB3, &m_pinB4, &m_pinB5, &m_pinB6, &m_pinB7, &m_pinB8 };

        if (dir == Level::High)
        {
            if (m_lastDir != Level::High)
            {
                for (int i = 0; i < 8; ++i)
                {
                    a[i]->release();
                }
            }
            m_lastDir = Level::High;
            for (int i = 0; i < 8; ++i)
            {
                const Level l = a[i]->read<Level>();
                if (l == Level::HighZ)
                {
                    b[i]->release();
                }
                else
                {
                    b[i]->drive<Level>(l);
                }
            }
        }
        else
        {
            if (m_lastDir != Level::Low)
            {
                for (int i = 0; i < 8; ++i)
                {
                    b[i]->release();
                }
            }
            m_lastDir = Level::Low;
            for (int i = 0; i < 8; ++i)
            {
                const Level l = b[i]->read<Level>();
                if (l == Level::HighZ)
                {
                    a[i]->release();
                }
                else
                {
                    a[i]->drive<Level>(l);
                }
            }
        }

        m_computing = false;
    }
} // namespace nfx::silicon::buffer::sn74xxx
