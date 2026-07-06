#include "nfx/silicon/chip/cdp1xxx/CDP1861.h"

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

    CDP1861::CDP1861(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinNCLK{ makeInput("nCLK") },
          m_pinNDMAO{ makeOutput("nDMAO") },
          m_pinNINT{ makeOutput("nINT") },
          m_pinTPA{ makeInput("TPA") },
          m_pinTPB{ makeInput("TPB") },
          m_pinNCOMPSYNC{ makeOutput("nCOMPSYNC") },
          m_pinVIDEO{ makeOutput("VIDEO") },
          m_pinNRESET{ makeInput("nRESET") },
          m_pinNEFX{ makeOutput("nEFX") },
          m_pinDISPON{ makeInput("DISPON") },
          m_pinDISPOFF{ makeInput("DISPOFF") },
          m_pinVSS{ makePower("VSS") },
          m_pinDI0{ makeInput("DI0") },
          m_pinDI1{ makeInput("DI1") },
          m_pinDI2{ makeInput("DI2") },
          m_pinDI3{ makeInput("DI3") },
          m_pinDI4{ makeInput("DI4") },
          m_pinDI5{ makeInput("DI5") },
          m_pinDI6{ makeInput("DI6") },
          m_pinDI7{ makeInput("DI7") },
          m_pinSC0{ makeInput("SC0") },
          m_pinSC1{ makeInput("SC1") },
          m_pinNCLEAR{ makeOutput("nCLEAR") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinNCLK,  &m_pinNDMAO,  &m_pinNINT, &m_pinTPA,    &m_pinTPB,     &m_pinNCOMPSYNC,
                     &m_pinVIDEO, &m_pinNRESET, &m_pinNEFX, &m_pinDISPON, &m_pinDISPOFF, &m_pinVSS,
                     &m_pinDI0,   &m_pinDI1,    &m_pinDI2,  &m_pinDI3,    &m_pinDI4,     &m_pinDI5,
                     &m_pinDI6,   &m_pinDI7,    &m_pinSC0,  &m_pinSC1,    &m_pinNCLEAR,  &m_pinVDD },
          m_diPtrs{ &m_pinDI0, &m_pinDI1, &m_pinDI2, &m_pinDI3, &m_pinDI4, &m_pinDI5, &m_pinDI6, &m_pinDI7 }
    {
        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
        m_pinNRESET.connect<Level>([this](const Level rst) { onReset(rst); });
        m_pinDISPON.connect<Level>([this](const Level lvl) { onDispOn(lvl); });
        m_pinDISPOFF.connect<Level>([this](const Level lvl) { onDispOff(lvl); });
        m_pinTPA.connect<Level>([this](const Level tpa) { onTPA(tpa); });
        m_pinTPB.connect<Level>([this](const Level tpb) { onTPB(tpb); });
        m_pinNCLK.connect<Level>([this](const Level clk) { onCLK(clk); });
    }

    const char* CDP1861::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CDP1861::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CDP1861", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CDP1861: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CDP1861::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::nCLK:
                return m_pinNCLK;
            case Terminal::nDMAO:
                return m_pinNDMAO;
            case Terminal::nINT:
                return m_pinNINT;
            case Terminal::TPA:
                return m_pinTPA;
            case Terminal::TPB:
                return m_pinTPB;
            case Terminal::nCOMPSYNC:
                return m_pinNCOMPSYNC;
            case Terminal::VIDEO:
                return m_pinVIDEO;
            case Terminal::nRESET:
                return m_pinNRESET;
            case Terminal::nEFX:
                return m_pinNEFX;
            case Terminal::DISPON:
                return m_pinDISPON;
            case Terminal::DISPOFF:
                return m_pinDISPOFF;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::DI0:
                return m_pinDI0;
            case Terminal::DI1:
                return m_pinDI1;
            case Terminal::DI2:
                return m_pinDI2;
            case Terminal::DI3:
                return m_pinDI3;
            case Terminal::DI4:
                return m_pinDI4;
            case Terminal::DI5:
                return m_pinDI5;
            case Terminal::DI6:
                return m_pinDI6;
            case Terminal::DI7:
                return m_pinDI7;
            case Terminal::SC0:
                return m_pinSC0;
            case Terminal::SC1:
                return m_pinSC1;
            case Terminal::nCLEAR:
                return m_pinNCLEAR;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CDP1861", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CDP1861: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CDP1861::pins() const
    {
        return m_pinPtrs;
    }

    void CDP1861::reset()
    {
        m_hCounter = 0;
        m_tpaAfterHSync = 0;
        m_vCounter = 0;
        m_shiftReg = 0x00;
        m_shiftBit = 8;
        m_tpbLoadArmed = false;
        m_dmaCount = 0;
        m_inDmaWindow = false;
        m_inHSync = false;
        m_displayEnabled = false;
        m_lastTPA = Level::HighZ;
        m_lastTPB = Level::HighZ;
        m_lastDISPON = Level::HighZ;
        m_lastCLK = Level::HighZ;
        updateOutputs();
    }

    void CDP1861::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CDP1861",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            m_pinNDMAO.release();
            m_pinNINT.release();
            m_pinNCOMPSYNC.release();
            m_pinVIDEO.release();
            m_pinNEFX.release();
            m_pinNCLEAR.release();
        }
        else if (!wasPowered && m_powered)
        {
            updateOutputs();
        }
    }

    void CDP1861::onReset(const Level rst)
    {
        if (!m_powered)
        {
            return;
        }

        if (rst == Level::Low)
        {
            m_hCounter = 0;
            m_tpaAfterHSync = 0;
            m_vCounter = 0;
            m_shiftReg = 0x00;
            m_shiftBit = 8;
            m_tpbLoadArmed = false;
            m_dmaCount = 0;
            m_inDmaWindow = false;
            m_inHSync = false;
            m_displayEnabled = false;
            m_lastTPA = Level::HighZ;
            m_lastTPB = Level::HighZ;
            m_lastCLK = Level::HighZ;

            // /RESET Low -> /CLEAR output Low
            m_pinNCLEAR.drive<Level>(Level::Low);

            m_pinNDMAO.drive<Level>(Level::High);
            m_pinNINT.drive<Level>(Level::High);
            m_pinNCOMPSYNC.drive<Level>(Level::High);
            m_pinVIDEO.drive<Level>(Level::Low);
            m_pinNEFX.drive<Level>(Level::High);
        }
        else
        {
            m_pinNCLEAR.drive<Level>(Level::High);
            updateOutputs();
        }
    }

    void CDP1861::onDispOn(const Level lvl)
    {
        if (!m_powered || m_pinNRESET.read<Level>() == Level::Low)
        {
            m_lastDISPON = lvl;
            return;
        }

        if (m_lastDISPON == Level::Low && lvl == Level::High)
        {
            m_displayEnabled = true;
            updateOutputs();
        }

        m_lastDISPON = lvl;
    }

    void CDP1861::onDispOff(const Level lvl)
    {
        if (!m_powered || m_pinNRESET.read<Level>() == Level::Low)
        {
            return;
        }

        if (lvl == Level::High && m_pinDISPON.read<Level>() == Level::Low)
        {
            m_displayEnabled = false;
            updateOutputs();
        }
    }

    void CDP1861::onTPA(const Level tpa)
    {
        if (!m_powered)
        {
            m_lastTPA = tpa;
            return;
        }

        if (m_pinNRESET.read<Level>() == Level::Low)
        {
            m_lastTPA = tpa;
            return;
        }

        if (m_lastTPA == Level::Low && tpa == Level::High)
        {
            const Level sc0 = m_pinSC0.read<Level>();
            const Level sc1 = m_pinSC1.read<Level>();

            if (m_inHSync)
            {
                const bool normalSync = (sc1 == Level::High && sc0 == Level::Low);
                if (!normalSync)
                {
                    m_inHSync = false;
                    m_tpaAfterHSync = 0;
                    m_pinNCOMPSYNC.drive<Level>(Level::High);
                    m_lastTPA = tpa;
                    return;
                }
            }

            m_hCounter++;

            if (m_inHSync)
            {
                m_tpaAfterHSync++;
                if (m_tpaAfterHSync == 2)
                {
                    m_inHSync = false;
                    m_pinNCOMPSYNC.drive<Level>(Level::High);

                    m_dmaCount = 0;
                    m_inDmaWindow = false;

                    if (m_displayEnabled)
                    {
                        const bool inDisplayWindow =
                            (m_vCounter >= k_efxLineOffset && m_vCounter < k_efxLineOffset + k_displayLines);
                        if (inDisplayWindow)
                        {
                            m_inDmaWindow = true;
                            m_pinNDMAO.drive<Level>(Level::Low);
                        }
                    }
                }
            }

            if (m_hCounter >= k_cyclesPerLine)
            {
                m_hCounter = 0;
                m_vCounter++;
                if (m_vCounter >= k_linesPerField)
                {
                    m_vCounter = 0;
                }
            }

            updateOutputs();
        }

        m_lastTPA = tpa;
    }

    void CDP1861::onTPB(const Level tpb)
    {
        if (!m_powered)
        {
            m_lastTPB = tpb;
            return;
        }

        if (m_pinNRESET.read<Level>() == Level::Low)
        {
            m_lastTPB = tpb;
            return;
        }

        if (m_lastTPB == Level::Low && tpb == Level::High)
        {
            m_tpbLoadArmed = true;
        }

        if (m_lastTPB == Level::High && tpb == Level::Low)
        {
            const Level sc0 = m_pinSC0.read<Level>();
            const Level sc1 = m_pinSC1.read<Level>();
            const uint16_t syncThreshold = (sc1 == Level::High && sc0 == Level::Low) ? 13u : 12u;

            if (m_hCounter >= syncThreshold && !m_inHSync)
            {
                m_inHSync = true;
                m_tpaAfterHSync = 0;
                m_pinNCOMPSYNC.drive<Level>(Level::Low);
            }
        }

        m_lastTPB = tpb;
    }

    void CDP1861::onCLK(const Level clk)
    {
        if (!m_powered)
        {
            m_lastCLK = clk;
            return;
        }

        if (m_pinNRESET.read<Level>() == Level::Low)
        {
            m_lastCLK = clk;
            return;
        }

        if (m_lastCLK == Level::High && clk == Level::Low)
        {
            const Level tpb = m_pinTPB.read<Level>();
            const Level sc0 = m_pinSC0.read<Level>();
            const Level sc1 = m_pinSC1.read<Level>();
            const Level dispon = m_pinDISPON.read<Level>();

            if (tpb == Level::High && dispon == Level::High && sc0 == Level::High && sc1 == Level::Low &&
                m_displayEnabled && m_inDmaWindow && m_tpbLoadArmed)
            {
                std::uint8_t val = 0;
                for (std::uint8_t i = 0; i < 8; ++i)
                {
                    if (m_diPtrs[i]->read<Level>() == Level::High)
                    {
                        val |= static_cast<std::uint8_t>(1u << i);
                    }
                }
                m_shiftReg = val;
                m_shiftBit = 0;
                m_tpbLoadArmed = false;
                m_dmaCount++;

                if (m_dmaCount >= k_dmaBytes)
                {
                    m_inDmaWindow = false;
                    m_pinNDMAO.drive<Level>(Level::High);
                }
            }

            if (m_shiftBit < 8)
            {
                const uint8_t bit = (m_shiftReg >> (7u - m_shiftBit)) & 1u;
                m_pinVIDEO.drive<Level>(bit ? Level::High : Level::Low);
                m_shiftBit++;
            }
            else
            {
                m_pinVIDEO.drive<Level>(Level::Low);
            }
        }

        m_lastCLK = clk;
    }

    void CDP1861::updateOutputs()
    {
        if (!m_powered)
        {
            return;
        }

        if (m_pinNRESET.read<Level>() == Level::Low)
        {
            return;
        }

        const uint16_t displayStart = k_efxLineOffset;
        const uint16_t displayEnd = displayStart + k_displayLines;

        const uint16_t intStart = displayStart - k_intLineOffset;
        const bool intActive = m_displayEnabled && (m_vCounter >= intStart && m_vCounter < displayStart);
        m_pinNINT.drive<Level>(intActive ? Level::Low : Level::High);

        const uint16_t efxPreStart = displayStart - k_efxLineOffset;
        const uint16_t efxPostEnd = displayEnd + k_efxLineOffset;
        const bool efxActive = m_displayEnabled && ((m_vCounter >= efxPreStart && m_vCounter < displayStart) ||
                                                    (m_vCounter >= displayEnd && m_vCounter < efxPostEnd));
        m_pinNEFX.drive<Level>(efxActive ? Level::Low : Level::High);

        if (!m_inDmaWindow)
        {
            m_pinNDMAO.drive<Level>(Level::High);
        }
    }
} // namespace nfx::silicon::chip::cdp1xxx
