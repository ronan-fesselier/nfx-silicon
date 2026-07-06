#include "nfx/silicon/chip/cdp1xxx/CDP1852.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::chip::cdp1xxx
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

    CDP1852::CDP1852(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinNCS1_CS1{ makeInput("nCS1_CS1") },
          m_pinMODE{ makeInput("MODE") },
          m_pinDI0{ makeInput("DI0") },
          m_pinDO0{ makeOutput("DO0") },
          m_pinDI1{ makeInput("DI1") },
          m_pinDO1{ makeOutput("DO1") },
          m_pinDI2{ makeInput("DI2") },
          m_pinDO2{ makeOutput("DO2") },
          m_pinDI3{ makeInput("DI3") },
          m_pinDO3{ makeOutput("DO3") },
          m_pinCLOCK{ makeInput("CLOCK") },
          m_pinVSS{ makePower("VSS") },
          m_pinCS2{ makeInput("CS2") },
          m_pinCLEAR{ makeInput("CLEAR") },
          m_pinDO4{ makeOutput("DO4") },
          m_pinDI4{ makeInput("DI4") },
          m_pinDO5{ makeOutput("DO5") },
          m_pinDI5{ makeInput("DI5") },
          m_pinDO6{ makeOutput("DO6") },
          m_pinDI6{ makeInput("DI6") },
          m_pinDO7{ makeOutput("DO7") },
          m_pinDI7{ makeInput("DI7") },
          m_pinNSR_SR{ makeOutput("nSR_SR") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinNCS1_CS1, &m_pinMODE,  &m_pinDI0, &m_pinDO0, &m_pinDI1,    &m_pinDO1,
                     &m_pinDI2,      &m_pinDO2,   &m_pinDI3, &m_pinDO3, &m_pinCLOCK,  &m_pinVSS,
                     &m_pinCS2,      &m_pinCLEAR, &m_pinDO4, &m_pinDI4, &m_pinDO5,    &m_pinDI5,
                     &m_pinDO6,      &m_pinDI6,   &m_pinDO7, &m_pinDI7, &m_pinNSR_SR, &m_pinVDD },
          m_diPtrs{ &m_pinDI0, &m_pinDI1, &m_pinDI2, &m_pinDI3, &m_pinDI4, &m_pinDI5, &m_pinDI6, &m_pinDI7 },
          m_doPtrs{ &m_pinDO0, &m_pinDO1, &m_pinDO2, &m_pinDO3, &m_pinDO4, &m_pinDO5, &m_pinDO6, &m_pinDO7 }
    {
        m_pinCLOCK.connect<Level>([this](const Level clk) { onClock(clk); });
        m_pinCLEAR.connect<Level>([this](const Level clr) { onClear(clr); });
        m_pinNCS1_CS1.connect<Level>([this](const Level) { updateOutputs(); });
        m_pinCS2.connect<Level>([this](const Level) { updateOutputs(); });
        m_pinMODE.connect<Level>([this](const Level) { updateOutputs(); });

        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CDP1852::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CDP1852::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CDP1852", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CDP1852: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CDP1852::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::nCS1_CS1:
                return m_pinNCS1_CS1;
            case Terminal::MODE:
                return m_pinMODE;
            case Terminal::DI0:
                return m_pinDI0;
            case Terminal::DO0:
                return m_pinDO0;
            case Terminal::DI1:
                return m_pinDI1;
            case Terminal::DO1:
                return m_pinDO1;
            case Terminal::DI2:
                return m_pinDI2;
            case Terminal::DO2:
                return m_pinDO2;
            case Terminal::DI3:
                return m_pinDI3;
            case Terminal::DO3:
                return m_pinDO3;
            case Terminal::CLOCK:
                return m_pinCLOCK;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::CS2:
                return m_pinCS2;
            case Terminal::CLEAR:
                return m_pinCLEAR;
            case Terminal::DO4:
                return m_pinDO4;
            case Terminal::DI4:
                return m_pinDI4;
            case Terminal::DO5:
                return m_pinDO5;
            case Terminal::DI5:
                return m_pinDI5;
            case Terminal::DO6:
                return m_pinDO6;
            case Terminal::DI6:
                return m_pinDI6;
            case Terminal::DO7:
                return m_pinDO7;
            case Terminal::DI7:
                return m_pinDI7;
            case Terminal::nSR_SR:
                return m_pinNSR_SR;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CDP1852", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CDP1852: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CDP1852::pins() const
    {
        return m_pinPtrs;
    }

    void CDP1852::reset()
    {
        m_register = 0x00;
        m_lastClock = Level::HighZ;
        // input mode: /SR/SR=High (inactive); output mode: SR//SR=Low (inactive)
        m_sr = (m_pinMODE.read<Level>() == Level::High) ? false : true;
        updateOutputs();
    }

    void CDP1852::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CDP1852",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            for (auto* p : m_doPtrs)
            {
                p->release();
            }
            m_pinNSR_SR.release();
        }
        else if (!wasPowered && m_powered)
        {
            updateOutputs();
        }
    }

    void CDP1852::onClock(const Level clk)
    {
        if (!m_powered)
        {
            return;
        }

        // latch on High-to-Low transition
        if (m_lastClock == Level::High && clk == Level::Low)
        {
            const bool outputMode = (m_pinMODE.read<Level>() == Level::High);

            if (outputMode)
            {
                // output mode: latch when CS1/CS1=Low AND CS2=High
                const Level cs1 = m_pinNCS1_CS1.read<Level>();
                const Level cs2 = m_pinCS2.read<Level>();
                if (cs1 == Level::Low && cs2 == Level::High)
                {
                    std::uint8_t val = 0;
                    for (std::uint8_t i = 0; i < 8; ++i)
                    {
                        if (m_diPtrs[i]->read<Level>() == Level::High)
                        {
                            val |= static_cast<std::uint8_t>(1u << i);
                        }
                    }
                    m_register = val;
                }
            }
            else
            {
                // input mode: always latch DI, set /SR/SR=Low
                std::uint8_t val = 0;
                for (std::uint8_t i = 0; i < 8; ++i)
                {
                    if (m_diPtrs[i]->read<Level>() == Level::High)
                    {
                        val |= static_cast<std::uint8_t>(1u << i);
                    }
                }
                m_register = val;
                m_sr = false; // /SR/SR=Low: data ready
            }

            updateOutputs();
        }

        m_lastClock = clk;
    }

    void CDP1852::onClear(const Level clr)
    {
        if (!m_powered)
        {
            return;
        }

        if (clr == Level::High)
        {
            m_register = 0x00;
            const bool outputMode = (m_pinMODE.read<Level>() == Level::High);
            // input: /SR/SR=High; output: SR//SR=Low
            m_sr = outputMode ? false : true;
            updateOutputs();
        }
    }

    void CDP1852::updateOutputs()
    {
        if (!m_powered)
        {
            return;
        }

        const bool outputMode = (m_pinMODE.read<Level>() == Level::High);
        const Level cs1 = m_pinNCS1_CS1.read<Level>();
        const Level cs2 = m_pinCS2.read<Level>();

        if (outputMode)
        {
            // output mode: DO always driven from register
            for (std::uint8_t i = 0; i < 8; ++i)
            {
                m_doPtrs[i]->drive<Level>((m_register >> i) & 1u ? Level::High : Level::Low);
            }
            // SR//SR: High when deselected (CS1/CS1=High or CS2=Low), Low when selected
            const bool selected = (cs1 == Level::Low && cs2 == Level::High);
            m_pinNSR_SR.drive<Level>(selected ? Level::Low : Level::High);
        }
        else
        {
            // input mode: DO enabled only when CS1/CS1=High AND CS2=High
            const bool selected = (cs1 == Level::High && cs2 == Level::High);
            if (selected)
            {
                for (std::uint8_t i = 0; i < 8; ++i)
                {
                    m_doPtrs[i]->drive<Level>((m_register >> i) & 1u ? Level::High : Level::Low);
                }
                // /SR/SR=High when selected (data transferred)
                m_pinNSR_SR.drive<Level>(Level::High);
            }
            else
            {
                for (auto* p : m_doPtrs)
                {
                    p->release();
                }
                // /SR/SR reflects m_sr state: Low=data ready, High=idle
                m_pinNSR_SR.drive<Level>(m_sr ? Level::High : Level::Low);
            }
        }
    }
} // namespace nfx::silicon::chip::cdp1xxx
