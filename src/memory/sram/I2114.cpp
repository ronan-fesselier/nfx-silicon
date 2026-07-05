#include "nfx/silicon/memory/sram/I2114.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::memory::sram
{
    namespace
    {
        using Kind = internal::runtime::error::Kind;

        signal::Pin makeInput(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } };
        }

        signal::Pin makeBidir(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{ .name = name,
                                                         .kind = signal::Pin::Kind::Digital,
                                                         .direction = signal::Pin::Direction::Bidirectional } };
        }

        signal::Pin makePower(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Input } };
        }
    } // namespace

    I2114::I2114(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinA6{ makeInput("A6") },
          m_pinA5{ makeInput("A5") },
          m_pinA4{ makeInput("A4") },
          m_pinA3{ makeInput("A3") },
          m_pinA0{ makeInput("A0") },
          m_pinA1{ makeInput("A1") },
          m_pinA2{ makeInput("A2") },
          m_pinCS{ makeInput("CS") },
          m_pinGND{ makePower("GND") },
          m_pinWE{ makeInput("WE") },
          m_pinIO4{ makeBidir("IO4") },
          m_pinIO3{ makeBidir("IO3") },
          m_pinIO2{ makeBidir("IO2") },
          m_pinIO1{ makeBidir("IO1") },
          m_pinA9{ makeInput("A9") },
          m_pinA8{ makeInput("A8") },
          m_pinA7{ makeInput("A7") },
          m_pinVCC{ makePower("VCC") },
          m_pinPtrs{ &m_pinA6, &m_pinA5,  &m_pinA4,  &m_pinA3,  &m_pinA0,  &m_pinA1, &m_pinA2, &m_pinCS, &m_pinGND,
                     &m_pinWE, &m_pinIO4, &m_pinIO3, &m_pinIO2, &m_pinIO1, &m_pinA9, &m_pinA8, &m_pinA7, &m_pinVCC },
          m_aPtrs{ &m_pinA0, &m_pinA1, &m_pinA2, &m_pinA3, &m_pinA4, &m_pinA5, &m_pinA6, &m_pinA7, &m_pinA8, &m_pinA9 },
          m_ioPtrs{ &m_pinIO1, &m_pinIO2, &m_pinIO3, &m_pinIO4 }
    {
        auto onChange = [this](const Level) { compute(); };

        m_pinA0.connect<Level>(onChange);
        m_pinA1.connect<Level>(onChange);
        m_pinA2.connect<Level>(onChange);
        m_pinA3.connect<Level>(onChange);
        m_pinA4.connect<Level>(onChange);
        m_pinA5.connect<Level>(onChange);
        m_pinA6.connect<Level>(onChange);
        m_pinA7.connect<Level>(onChange);
        m_pinA8.connect<Level>(onChange);
        m_pinA9.connect<Level>(onChange);
        m_pinCS.connect<Level>(onChange);
        m_pinWE.connect<Level>([this](const Level lvl) { onWE(lvl); });

        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* I2114::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& I2114::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "I2114", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "I2114: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& I2114::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A6:
                return m_pinA6;
            case Terminal::A5:
                return m_pinA5;
            case Terminal::A4:
                return m_pinA4;
            case Terminal::A3:
                return m_pinA3;
            case Terminal::A0:
                return m_pinA0;
            case Terminal::A1:
                return m_pinA1;
            case Terminal::A2:
                return m_pinA2;
            case Terminal::CS:
                return m_pinCS;
            case Terminal::GND:
                return m_pinGND;
            case Terminal::WE:
                return m_pinWE;
            case Terminal::IO4:
                return m_pinIO4;
            case Terminal::IO3:
                return m_pinIO3;
            case Terminal::IO2:
                return m_pinIO2;
            case Terminal::IO1:
                return m_pinIO1;
            case Terminal::A9:
                return m_pinA9;
            case Terminal::A8:
                return m_pinA8;
            case Terminal::A7:
                return m_pinA7;
            case Terminal::VCC:
                return m_pinVCC;
        }
        internal::runtime::error::log(
            "I2114", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "I2114: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> I2114::pins() const
    {
        return m_pinPtrs;
    }

    void I2114::reset()
    {
        m_ram.fill(0x00);
        m_wePrev = Level::High;
        for (auto* p : m_ioPtrs)
        {
            p->release();
        }
    }

    void I2114::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "I2114",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: outputs held at HighZ");
            for (auto* p : m_ioPtrs)
            {
                p->release();
            }
        }
        else if (!wasPowered && m_powered)
        {
            compute();
        }
    }

    void I2114::onWE(const Level lvl)
    {
        const Level prev = m_wePrev;
        m_wePrev = lvl;

        if (!m_powered)
        {
            return;
        }

        const Level s = m_pinCS.read<Level>();

        if (s != Level::Low)
        {
            return;
        }

        if (lvl == Level::Low)
        {
            // WE asserted: output buffers inhibited (t_otw), chip reads IO passively
            return;
        }

        if (lvl == Level::High && prev == Level::Low)
        {
            // WE rising edge: data has been stable for t_dw, capture now
            std::uint16_t addr = 0;
            for (std::uint8_t i = 0; i < 10; ++i)
            {
                if (m_aPtrs[i]->read<Level>() == Level::High)
                {
                    addr |= static_cast<std::uint16_t>(1u << i);
                }
            }
            std::uint8_t nibble = 0;
            for (std::uint8_t i = 0; i < 4; ++i)
            {
                if (m_ioPtrs[i]->read<Level>() == Level::High)
                {
                    nibble |= static_cast<std::uint8_t>(1u << i);
                }
            }
            m_ram[addr] = nibble;
        }

        compute();
    }

    void I2114::compute()
    {
        if (!m_powered)
        {
            return;
        }

        const Level s = m_pinCS.read<Level>();

        if (s != Level::Low)
        {
            for (auto* p : m_ioPtrs)
            {
                p->release();
            }
            return;
        }

        std::uint16_t addr = 0;
        for (std::uint8_t i = 0; i < 10; ++i)
        {
            const Level bit = m_aPtrs[i]->read<Level>();
            if (bit == Level::HighZ)
            {
                for (auto* p : m_ioPtrs)
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

        const Level w = m_pinWE.read<Level>();

        if (w == Level::HighZ)
        {
            // WE floating: undefined
            for (auto* p : m_ioPtrs)
            {
                p->release();
            }
            return;
        }

        if (w == Level::Low)
        {
            // WE active: output buffers inhibited (t_otw), write pending until WE rising edge
            return;
        }

        const std::uint8_t nibble = m_ram[addr];
        for (std::uint8_t i = 0; i < 4; ++i)
        {
            m_ioPtrs[i]->drive<Level>((nibble >> i) & 1u ? Level::High : Level::Low);
        }
    }
} // namespace nfx::silicon::memory::sram
