#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"

#include "internal/CDP1802/Spec.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
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

        signal::Pin makeBidir(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{ .name = name,
                                                         .kind = signal::Pin::Kind::Digital,
                                                         .direction = signal::Pin::Direction::Bidirectional } };
        }
    } // namespace

    CDP1802::CDP1802(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinCLOCK{ makeInput("CLOCK") },
          m_pinNWAIT{ makeInput("nWAIT") },
          m_pinNCLEAR{ makeInput("nCLEAR") },
          m_pinQ{ makeOutput("Q") },
          m_pinSC1{ makeOutput("SC1") },
          m_pinSC0{ makeOutput("SC0") },
          m_pinNMRD{ makeOutput("nMRD") },
          m_pinBUS7{ makeBidir("BUS7") },
          m_pinBUS6{ makeBidir("BUS6") },
          m_pinBUS5{ makeBidir("BUS5") },
          m_pinBUS4{ makeBidir("BUS4") },
          m_pinBUS3{ makeBidir("BUS3") },
          m_pinBUS2{ makeBidir("BUS2") },
          m_pinBUS1{ makeBidir("BUS1") },
          m_pinBUS0{ makeBidir("BUS0") },
          m_pinVCC{ makePower("VCC") },
          m_pinN2{ makeOutput("N2") },
          m_pinN1{ makeOutput("N1") },
          m_pinN0{ makeOutput("N0") },
          m_pinVSS{ makePower("VSS") },
          m_pinNEF4{ makeInput("nEF4") },
          m_pinNEF3{ makeInput("nEF3") },
          m_pinNEF2{ makeInput("nEF2") },
          m_pinNEF1{ makeInput("nEF1") },
          m_pinMA0{ makeOutput("MA0") },
          m_pinMA1{ makeOutput("MA1") },
          m_pinMA2{ makeOutput("MA2") },
          m_pinMA3{ makeOutput("MA3") },
          m_pinMA4{ makeOutput("MA4") },
          m_pinMA5{ makeOutput("MA5") },
          m_pinMA6{ makeOutput("MA6") },
          m_pinMA7{ makeOutput("MA7") },
          m_pinTPB{ makeOutput("TPB") },
          m_pinTPA{ makeOutput("TPA") },
          m_pinNMWR{ makeOutput("nMWR") },
          m_pinNINT{ makeInput("nINT") },
          m_pinNDMAO{ makeInput("nDMAO") },
          m_pinNDMAI{ makeInput("nDMAI") },
          m_pinNXTAL{ makeInput("nXTAL") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinCLOCK, &m_pinNWAIT, &m_pinNCLEAR, &m_pinQ,     &m_pinSC1,  &m_pinSC0,  &m_pinNMRD,
                     &m_pinBUS7,  &m_pinBUS6,  &m_pinBUS5,   &m_pinBUS4,  &m_pinBUS3, &m_pinBUS2, &m_pinBUS1,
                     &m_pinBUS0,  &m_pinVCC,   &m_pinN2,     &m_pinN1,    &m_pinN0,   &m_pinVSS,  &m_pinNEF4,
                     &m_pinNEF3,  &m_pinNEF2,  &m_pinNEF1,   &m_pinMA0,   &m_pinMA1,  &m_pinMA2,  &m_pinMA3,
                     &m_pinMA4,   &m_pinMA5,   &m_pinMA6,    &m_pinMA7,   &m_pinTPB,  &m_pinTPA,  &m_pinNMWR,
                     &m_pinNINT,  &m_pinNDMAO, &m_pinNDMAI,  &m_pinNXTAL, &m_pinVDD },
          m_busPtrs{ &m_pinBUS0, &m_pinBUS1, &m_pinBUS2, &m_pinBUS3, &m_pinBUS4, &m_pinBUS5, &m_pinBUS6, &m_pinBUS7 },
          m_maPtrs{ &m_pinMA0, &m_pinMA1, &m_pinMA2, &m_pinMA3, &m_pinMA4, &m_pinMA5, &m_pinMA6, &m_pinMA7 }
    {
        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
        m_pinCLOCK.connect<Level>([this](const Level clk) { onClock(clk); });
        m_pinNCLEAR.connect<Level>([this](const Level lvl) { onNClear(lvl); });
        m_pinNWAIT.connect<Level>([this](const Level lvl) { onNWait(lvl); });
        m_pinNDMAI.connect<Level>([this](const Level lvl) { onNDMAI(lvl); });
        m_pinNDMAO.connect<Level>([this](const Level lvl) { onNDMAO(lvl); });
        m_pinNINT.connect<Level>([this](const Level lvl) { onNINT(lvl); });
    }

    const char* CDP1802::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CDP1802::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CDP1802", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CDP1802: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CDP1802::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::CLOCK:
                return m_pinCLOCK;
            case Terminal::nWAIT:
                return m_pinNWAIT;
            case Terminal::nCLEAR:
                return m_pinNCLEAR;
            case Terminal::Q:
                return m_pinQ;
            case Terminal::SC1:
                return m_pinSC1;
            case Terminal::SC0:
                return m_pinSC0;
            case Terminal::nMRD:
                return m_pinNMRD;
            case Terminal::BUS7:
                return m_pinBUS7;
            case Terminal::BUS6:
                return m_pinBUS6;
            case Terminal::BUS5:
                return m_pinBUS5;
            case Terminal::BUS4:
                return m_pinBUS4;
            case Terminal::BUS3:
                return m_pinBUS3;
            case Terminal::BUS2:
                return m_pinBUS2;
            case Terminal::BUS1:
                return m_pinBUS1;
            case Terminal::BUS0:
                return m_pinBUS0;
            case Terminal::VCC:
                return m_pinVCC;
            case Terminal::N2:
                return m_pinN2;
            case Terminal::N1:
                return m_pinN1;
            case Terminal::N0:
                return m_pinN0;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::nEF4:
                return m_pinNEF4;
            case Terminal::nEF3:
                return m_pinNEF3;
            case Terminal::nEF2:
                return m_pinNEF2;
            case Terminal::nEF1:
                return m_pinNEF1;
            case Terminal::MA0:
                return m_pinMA0;
            case Terminal::MA1:
                return m_pinMA1;
            case Terminal::MA2:
                return m_pinMA2;
            case Terminal::MA3:
                return m_pinMA3;
            case Terminal::MA4:
                return m_pinMA4;
            case Terminal::MA5:
                return m_pinMA5;
            case Terminal::MA6:
                return m_pinMA6;
            case Terminal::MA7:
                return m_pinMA7;
            case Terminal::TPB:
                return m_pinTPB;
            case Terminal::TPA:
                return m_pinTPA;
            case Terminal::nMWR:
                return m_pinNMWR;
            case Terminal::nINT:
                return m_pinNINT;
            case Terminal::nDMAO:
                return m_pinNDMAO;
            case Terminal::nDMAI:
                return m_pinNDMAI;
            case Terminal::nXTAL:
                return m_pinNXTAL;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CDP1802", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CDP1802: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CDP1802::pins() const
    {
        return m_pinPtrs;
    }

    void CDP1802::reset()
    {
        m_registers.D = 0x00;
        m_registers.DF = false;
        m_registers.B = 0x00;
        m_registers.R = {};
        m_registers.P = 0;
        m_registers.X = 0;
        m_registers.N = 0;
        m_registers.I = 0;
        m_registers.T = 0x00;
        m_registers.IE = true;
        m_registers.Q = false;

        m_clock.count = 0;
        m_clock.last = Level::HighZ;

        m_control.clearPending = false;
        m_control.waitActive = false;

        m_io.dmaIn = false;
        m_io.dmaOut = false;
        m_io.interrupt = false;
        m_io.efSampled[0] = m_io.efSampled[1] = m_io.efSampled[2] = m_io.efSampled[3] = false;

        m_exec.state = ExecutionState::State::Fetch;
        m_exec.idling = false;
        m_exec.extraExecuteCycles = 0;
        m_exec.executePhase = 0;

        if (!m_powered)
        {
            return;
        }

        m_pinQ.drive(Level::Low);
        m_pinSC1.drive(Level::Low);
        m_pinSC0.drive(Level::Low);
        m_pinNMRD.drive(Level::High);
        m_pinNMWR.drive(Level::High);
        m_pinTPA.drive(Level::Low);
        m_pinTPB.drive(Level::Low);
        m_pinN0.drive(Level::Low);
        m_pinN1.drive(Level::Low);
        m_pinN2.drive(Level::Low);
        releaseBus();
        driveAddress(0x0000, true);
        driveAddress(0x0000, false);
    }

    std::vector<component::InspectValue> CDP1802::inspect() const
    {
        std::vector<component::InspectValue> out;
        out.reserve(22);

        out.push_back({ "D", static_cast<uint32_t>(m_registers.D), 8 });
        out.push_back({ "DF", m_registers.DF ? 1u : 0u, 1 });

        static char rnames[16][4]; // static: names must outlive the returned vector
        for (int i = 0; i < 16; ++i)
        {
            std::snprintf(rnames[i], sizeof(rnames[i]), "R%X", i);
            out.push_back({ rnames[i], static_cast<uint32_t>(m_registers.R[static_cast<size_t>(i)]), 16 });
        }

        out.push_back({ "P", static_cast<uint32_t>(m_registers.P), 4 });
        out.push_back({ "X", static_cast<uint32_t>(m_registers.X), 4 });
        out.push_back({ "T", static_cast<uint32_t>(m_registers.T), 8 });
        out.push_back({ "IE", m_registers.IE ? 1u : 0u, 1 });
        out.push_back({ "Q", m_registers.Q ? 1u : 0u, 1 });

        return out;
    }

    std::optional<component::InspectValue> CDP1802::inspect(const char* name) const
    {
        if (std::strcmp(name, "D") == 0)
        {
            return component::InspectValue{ "D", static_cast<uint32_t>(m_registers.D), 8 };
        }
        if (std::strcmp(name, "DF") == 0)
        {
            return component::InspectValue{ "DF", m_registers.DF ? 1u : 0u, 1 };
        }
        if (std::strcmp(name, "P") == 0)
        {
            return component::InspectValue{ "P", static_cast<uint32_t>(m_registers.P), 4 };
        }
        if (std::strcmp(name, "X") == 0)
        {
            return component::InspectValue{ "X", static_cast<uint32_t>(m_registers.X), 4 };
        }
        if (std::strcmp(name, "T") == 0)
        {
            return component::InspectValue{ "T", static_cast<uint32_t>(m_registers.T), 8 };
        }
        if (std::strcmp(name, "IE") == 0)
        {
            return component::InspectValue{ "IE", m_registers.IE ? 1u : 0u, 1 };
        }
        if (std::strcmp(name, "Q") == 0)
        {
            return component::InspectValue{ "Q", m_registers.Q ? 1u : 0u, 1 };
        }

        if (name[0] == 'R' && name[1] != '\0')
        {
            char* end = nullptr;
            const long idx = std::strtol(name + 1, &end, 16);
            if (end != name + 1 && *end == '\0' && idx >= 0 && idx <= 15)
            {
                static char rname[4];
                std::snprintf(rname, sizeof(rname), "R%lX", idx);
                return component::InspectValue{ rname,
                                                static_cast<uint32_t>(m_registers.R[static_cast<size_t>(idx)]),
                                                16 };
            }
        }

        return std::nullopt;
    }

    void CDP1802::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CDP1802",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            m_pinQ.release();
            m_pinSC1.release();
            m_pinSC0.release();
            m_pinNMRD.release();
            m_pinNMWR.release();
            m_pinTPA.release();
            m_pinTPB.release();
            m_pinN0.release();
            m_pinN1.release();
            m_pinN2.release();
            releaseBus();
            for (auto* p : m_maPtrs)
            {
                p->release();
            }
        }
        else if (!wasPowered && m_powered)
        {
            reset();
        }
    }

    void CDP1802::onClock(const Level clk)
    {
        if (!m_powered)
        {
            return;
        }
        if (m_clock.last != Level::High && clk == Level::High)
        {
            tickClock();
        }
        m_clock.last = clk;
    }

    void CDP1802::onNClear(const Level lvl)
    {
        if (!m_powered)
        {
            return;
        }
        m_control.clearPending = (lvl == Level::Low);

        if (m_control.clearPending)
        {
            m_pinTPA.drive(Level::Low);
            m_pinTPB.drive(Level::Low);
            m_pinNMRD.drive(Level::High);
            m_pinNMWR.drive(Level::High);
            m_registers.I = 0;
            m_registers.N = 0;
            m_registers.Q = false;
            m_registers.IE = true;
            m_registers.P = 0;
            m_registers.X = 0;
            m_registers.R[0] = 0x0000;
            m_clock.count = 0;
            m_exec.state = ExecutionState::State::Fetch;
            m_exec.idling = false;
            m_exec.extraExecuteCycles = 0;
            m_exec.executePhase = 0;
            updateQ();
            updateSC();
            updateN(0);
        }
    }

    void CDP1802::onNWait(const Level lvl)
    {
        if (!m_powered)
        {
            return;
        }
        m_control.waitActive = (lvl == Level::Low);
    }

    void CDP1802::onNDMAI(const Level lvl)
    {
        if (!m_powered)
        {
            return;
        }
        m_io.dmaIn = (lvl == Level::Low);
    }

    void CDP1802::onNDMAO(const Level lvl)
    {
        if (!m_powered)
        {
            return;
        }
        m_io.dmaOut = (lvl == Level::Low);
    }

    void CDP1802::onNINT(const Level lvl)
    {
        if (!m_powered)
        {
            return;
        }
        m_io.interrupt = (lvl == Level::Low);
    }

    void CDP1802::tickClock()
    {
        if (m_control.clearPending || m_control.waitActive)
        {
            return;
        }

        const uint8_t count = m_clock.count;

        switch (count)
        {
            case 0:
            {
                if (m_exec.didWrite)
                {
                    m_pinNMWR.drive(Level::High);
                    releaseBus();
                    m_exec.didWrite = false;
                }
                beginCycle();
                break;
            }
            case 1:
            {
                m_pinTPA.drive(Level::High);
                break;
            }
            case 2:
            {
                m_pinTPA.drive(Level::Low);
                driveAddress(
                    (m_exec.state == ExecutionState::State::DmaIn || m_exec.state == ExecutionState::State::DmaOut)
                        ? m_registers.R[0]
                        : m_registers.R[m_registers.P],
                    false);
                break;
            }
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
            {
                m_pinTPB.drive(Level::High);
                break;
            }
            case 7:
            {
                m_pinTPB.drive(Level::Low);
                endCycle();
                break;
            }
            default:
                break;
        }

        m_clock.count = static_cast<uint8_t>((count + 1) & 0x07u);
    }

    void CDP1802::beginCycle()
    {
        using S = ExecutionState::State;

        // SC1/SC0 and Q reflect state decided at end of previous cycle
        updateSC();
        updateQ();

        // N0..N2 valid from the start of S1, cleared for all other states
        updateN(m_exec.state == S::Execute ? m_registers.N : 0u);

        // Address bus: high byte from the register appropriate for this cycle type
        const uint16_t addr =
            (m_exec.state == S::DmaIn || m_exec.state == S::DmaOut) ? m_registers.R[0] : m_registers.R[m_registers.P];
        driveAddress(addr, true);

        // nMRD: Low for any read cycle (Fetch, Execute, DmaOut), High for DmaIn and Interrupt
        const bool isRead = (m_exec.state == S::Fetch || m_exec.state == S::Execute || m_exec.state == S::DmaOut);
        m_pinNMRD.drive(isRead ? Level::Low : Level::High);
    }

    void CDP1802::endCycle()
    {
        using S = ExecutionState::State;

        // Extra execute cycles (Cycles{3}+): re-dispatch with incremented phase
        if (m_exec.state == S::Execute && m_exec.extraExecuteCycles > 0)
        {
            --m_exec.extraExecuteCycles;
            ++m_exec.executePhase;
            execute();
        }
        else
        {
            // Execute the handler for the cycle that just completed
            switch (m_exec.state)
            {
                case S::Fetch:
                    fetch();
                    break;
                case S::Execute:
                    execute();
                    break;
                case S::DmaIn:
                    dmaIn();
                    break;
                case S::DmaOut:
                    dmaOut();
                    break;
                case S::Interrupt:
                    interrupt();
                    break;
            }
        }

        // Decide the next cycle type, checked after each handler may clear flags
        if (m_exec.idling)
        {
            if (m_io.dmaIn)
            {
                m_exec.idling = false;
                m_exec.state = S::DmaIn;
                return;
            }
            if (m_io.dmaOut)
            {
                m_exec.idling = false;
                m_exec.state = S::DmaOut;
                return;
            }
            if (m_io.interrupt && m_registers.IE)
            {
                m_exec.idling = false;
                m_exec.state = S::Interrupt;
                return;
            }
            m_exec.state = S::Execute; // stay idle: repeat S1 read-and-discard
            return;
        }

        if (m_exec.state == S::Fetch)
        {
            // After fetch, always execute next
            m_exec.state = S::Execute;
            return;
        }

        // Extra execute cycles set by execute(): stay in Execute, next endCycle() consumes silently
        if (m_exec.state == S::Execute && m_exec.extraExecuteCycles > 0)
        {
            return;
        }

        // After Execute (or DMA/INT): check for DMA/INT, else Fetch
        if (m_io.dmaIn)
        {
            m_exec.state = S::DmaIn;
            return;
        }
        if (m_io.dmaOut)
        {
            m_exec.state = S::DmaOut;
            return;
        }
        if (m_io.interrupt && m_registers.IE)
        {
            m_exec.state = S::Interrupt;
            return;
        }
        m_exec.state = S::Fetch;
    }

    void CDP1802::fetch()
    {
        const uint8_t byte = readBus();
        m_registers.I = static_cast<uint8_t>((byte >> 4) & 0x0Fu);
        m_registers.N = static_cast<uint8_t>(byte & 0x0Fu);
        m_registers.R[m_registers.P]++;
        m_exec.executePhase = 0;
    }

    void CDP1802::execute()
    {
        m_io.efSampled[0] = (m_pinNEF1.read<Level>() == Level::Low);
        m_io.efSampled[1] = (m_pinNEF2.read<Level>() == Level::Low);
        m_io.efSampled[2] = (m_pinNEF3.read<Level>() == Level::Low);
        m_io.efSampled[3] = (m_pinNEF4.read<Level>() == Level::Low);

        const uint8_t opcode = static_cast<uint8_t>((m_registers.I << 4u) | m_registers.N);
        const Cycles cycles = cdp1802internal::cdp1802Dispatch(*this, opcode);

        if (m_exec.executePhase == 0 && cycles > Cycles{ 2 })
        {
            m_exec.extraExecuteCycles = static_cast<uint8_t>(cycles - 2u);
        }
    }

    void CDP1802::dmaIn()
    {
        m_pinNMWR.drive(Level::Low);
        m_pinNMWR.drive(Level::High);
        m_registers.R[0]++;
        m_io.dmaIn = false;
        releaseBus();
    }

    void CDP1802::dmaOut()
    {
        m_pinNMRD.drive(Level::Low);
        m_registers.R[0]++;
        m_io.dmaOut = false;
    }

    void CDP1802::interrupt()
    {
        m_registers.T = static_cast<uint8_t>((m_registers.X << 4u) | m_registers.P);
        m_registers.IE = false;
        m_registers.P = 1;
        m_registers.X = 2;
        m_io.interrupt = false;
        releaseBus();
    }

    void CDP1802::driveAddress(const uint16_t addr, const bool highByte)
    {
        const uint8_t byte = highByte ? static_cast<uint8_t>((addr >> 8) & 0xFFu) : static_cast<uint8_t>(addr & 0xFFu);

        for (int i = 0; i < 8; ++i)
        {
            m_maPtrs[static_cast<size_t>(i)]->drive((byte & (1u << i)) ? Level::High : Level::Low);
        }
    }

    void CDP1802::driveBus(const uint8_t data)
    {
        for (int i = 0; i < 8; ++i)
        {
            m_busPtrs[static_cast<size_t>(i)]->drive((data & (1u << i)) ? Level::High : Level::Low);
        }
    }

    uint8_t CDP1802::readBus() const
    {
        uint8_t result = 0;
        for (int i = 0; i < 8; ++i)
        {
            if (m_busPtrs[static_cast<size_t>(i)]->read<Level>() == Level::High)
            {
                result |= static_cast<uint8_t>(1u << i);
            }
        }
        return result;
    }

    void CDP1802::releaseBus()
    {
        for (auto* p : m_busPtrs)
        {
            p->release();
        }
    }

    void CDP1802::updateSC()
    {
        using S = ExecutionState::State;
        bool sc1 = false;
        bool sc0 = false;

        switch (m_exec.state)
        {
            case S::Fetch:
                break; // S0
            case S::Execute:
                sc0 = true;
                break; // S1
            case S::DmaIn:
            case S::DmaOut:
                sc1 = true;
                break; // S2
            case S::Interrupt:
                sc1 = true;
                sc0 = true;
                break; // S3
        }

        m_pinSC1.drive(sc1 ? Level::High : Level::Low);
        m_pinSC0.drive(sc0 ? Level::High : Level::Low);
    }

    void CDP1802::updateQ()
    {
        m_pinQ.drive(m_registers.Q ? Level::High : Level::Low);
    }

    void CDP1802::updateN(const uint8_t n)
    {
        m_pinN0.drive((n & 0x01u) ? Level::High : Level::Low);
        m_pinN1.drive((n & 0x02u) ? Level::High : Level::Low);
        m_pinN2.drive((n & 0x04u) ? Level::High : Level::Low);
    }
} // namespace nfx::silicon::chip::cdp1xxx
