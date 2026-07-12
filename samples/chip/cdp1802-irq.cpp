/*
 * CDP1802 periodic interrupt sample
 *
 * A software /INT controller asserts nINT every k_tickInterval machine cycles.
 * The ISR counts k_targetTicks ticks before incrementing R5.
 * When R5 == k_maxCount the program halts at IDL.
 *
 * Memory layout (256-byte software stub):
 *   0x000  init: R1=0x0014 (INT_VECTOR), R2=0x007F (stack), R6=0x00FF (TICK addr)
 *   0x00E  SEX R0 / RET / DB $00   enable IE, restore X=0 P=0
 *   0x011  BR 0x011                spin
 *   0x013  RET   EXIT_INT          one byte before INT_VECTOR
 *   0x014  INT_VECTOR              DEC R2 / SAV / TICK logic / INC R5 / IDL or loop
 *   0x027  IDL
 *
 * R1 drift: CDP1802 does not reset R1 on interrupt entry, R1 advances
 * during ISR execution. EXIT_INT (RET at 0x013) fixes this: fetching the
 * RET leaves R1=0x014, resetting R1 to INT_VECTOR on each ISR return.
 * RET reads M(R2) (pushed by SAV) to restore X and P, then sets IE=1.
 *
 * Memory bus protocol (software stub, no discrete memory ICs):
 *   nMRD low at clock 0, MA low byte stable at clock 2
 *   TPB high at clock 6: stub drives data on BUS
 *   nMWR low at clock 7: MA and BUS already valid, stub captures immediately
 *
 * nINT held low for 2 machine cycles: Execute cycle that samples IE
 * and S3 cycle that commits the interrupt.
 */

#include <nfx/Silicon.h>
#include <array>
#include <cstdint>
#include <iostream>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::chip::cdp1xxx;

namespace
{
    constexpr int k_tickInterval = 200;
    constexpr uint8_t k_targetTicks = 10;
    constexpr uint8_t k_maxCount = 5;

    constexpr uint8_t k_intVector = 0x14;
    constexpr uint8_t k_stackPtr = 0x7F;
    constexpr uint8_t k_tickAddr = 0xFF;

    // clang-format off
    constexpr CDP1802::Terminal k_bus[] = {
        CDP1802::Terminal::BUS0,
        CDP1802::Terminal::BUS1,
        CDP1802::Terminal::BUS2,
        CDP1802::Terminal::BUS3,
        CDP1802::Terminal::BUS4,
        CDP1802::Terminal::BUS5,
        CDP1802::Terminal::BUS6,
        CDP1802::Terminal::BUS7
    };
    constexpr CDP1802::Terminal k_ma[] = {
        CDP1802::Terminal::MA0,
        CDP1802::Terminal::MA1,
        CDP1802::Terminal::MA2,
        CDP1802::Terminal::MA3,
        CDP1802::Terminal::MA4,
        CDP1802::Terminal::MA5,
        CDP1802::Terminal::MA6,
        CDP1802::Terminal::MA7
    };
    // clang-format on

    std::array<uint8_t, 256> g_mem{};

    auto buildMem() -> void
    {
        g_mem.fill(0);

        // clang-format off
        g_mem[0x000] = 0xF8; g_mem[0x001] = 0x00;          // LDI 0x00
        g_mem[0x002] = 0xB1;                               // PHI R1
        g_mem[0x003] = 0xB2;                               // PHI R2
        g_mem[0x004] = 0xB6;                               // PHI R6
        g_mem[0x005] = 0xF8; g_mem[0x006] = k_intVector;   // LDI k_intVector
        g_mem[0x007] = 0xA1;                               // PLO R1
        g_mem[0x008] = 0xF8; g_mem[0x009] = k_stackPtr;    // LDI k_stackPtr
        g_mem[0x00A] = 0xA2;                               // PLO R2
        g_mem[0x00B] = 0xF8; g_mem[0x00C] = k_tickAddr;    // LDI k_tickAddr
        g_mem[0x00D] = 0xA6;                               // PLO R6
        g_mem[0x00E] = 0xE0;                               // SEX R0
        g_mem[0x00F] = 0x70; g_mem[0x010] = 0x00;          // RET  (X=0 P=0 IE=1)
        g_mem[0x011] = 0x30; g_mem[0x012] = 0x11;          // BR 0x011  (spin)
        g_mem[0x013] = 0x70;                               // RET  EXIT_INT (fetch leaves R1=0x014)
        g_mem[0x014] = 0x22;                               // DEC R2    INT_VECTOR
        g_mem[0x015] = 0x78;                               // SAV       M(R2) = T
        g_mem[0x016] = 0x06;                               // LDN R6    D = TICK
        g_mem[0x017] = 0xFC; g_mem[0x018] = 0x01;          // ADI 1     D = TICK+1
        g_mem[0x019] = 0x56;                               // STR R6    TICK = D
        g_mem[0x01A] = 0xFB; g_mem[0x01B] = k_targetTicks; // XRI k_targetTicks
        g_mem[0x01C] = 0x3A; g_mem[0x01D] = 0x13;          // BNZ 0x013 exit ISR if not at target
        g_mem[0x01E] = 0x56;                               // STR R6    TICK = 0 (D=0 after XRI hit)
        g_mem[0x01F] = 0x15;                               // INC R5
        g_mem[0x020] = 0x85;                               // GLO R5    D = R5.lo
        g_mem[0x021] = 0xFB; g_mem[0x022] = k_maxCount;    // XRI k_maxCount
        g_mem[0x023] = 0x32; g_mem[0x024] = 0x27;          // BZ 0x027  halt if R5 == k_maxCount
        g_mem[0x025] = 0x30; g_mem[0x026] = 0x13;          // BR 0x013  exit ISR
        g_mem[0x027] = 0x00;                               // IDL
        // clang-format on
    }

    auto readByte(CDP1802& cpu) -> uint8_t
    {
        uint8_t v = 0;
        for (int i = 0; i < 8; ++i)
        {
            if (cpu.pin(k_bus[i]).read<Level>() == Level::High)
            {
                v |= static_cast<uint8_t>(1u << i);
            }
        }
        return v;
    }

    auto readAddr(CDP1802& cpu) -> uint8_t
    {
        uint8_t v = 0;
        for (int i = 0; i < 8; ++i)
        {
            if (cpu.pin(k_ma[i]).read<Level>() == Level::High)
            {
                v |= static_cast<uint8_t>(1u << i);
            }
        }
        return v;
    }

    auto driveBus(CDP1802& cpu, uint8_t val) -> void
    {
        for (int i = 0; i < 8; ++i)
        {
            cpu.pin(k_bus[i]).drive<Level>((val & (1u << i)) ? Level::High : Level::Low);
        }
    }

    auto releaseBus(CDP1802& cpu) -> void
    {
        for (auto t : k_bus)
        {
            cpu.pin(t).release();
        }
    }

    auto clockOnce(CDP1802& cpu) -> void
    {
        cpu.pin(CDP1802::Terminal::CLOCK).drive<Level>(Level::High);
        cpu.pin(CDP1802::Terminal::CLOCK).drive<Level>(Level::Low);
    }
} // namespace

int main()
{
    buildMem();
    CDP1802 cpu{ CDP1802::Descriptor{ "U1" } };

    cpu.pin(CDP1802::Terminal::VDD).drive<float>(5.0f);
    cpu.pin(CDP1802::Terminal::nCLEAR).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::nWAIT).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::nINT).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::nDMAI).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::nDMAO).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::CLOCK).drive<Level>(Level::Low);

    // Memory bus stub: reads on TPB rising edge, writes on nMWR falling edge
    bool readCycleActive = false;

    cpu.pin(CDP1802::Terminal::nMRD).connect<Level>([&](Level lvl) {
        if (lvl == Level::Low)
        {
            readCycleActive = true;
        }
        else
        {
            readCycleActive = false;
            releaseBus(cpu);
        }
    });

    cpu.pin(CDP1802::Terminal::TPB).connect<Level>([&](Level lvl) {
        if (lvl == Level::High && readCycleActive)
        {
            driveBus(cpu, g_mem[readAddr(cpu)]);
        }
    });

    cpu.pin(CDP1802::Terminal::nMWR).connect<Level>([&](Level lvl) {
        if (lvl == Level::Low)
        {
            g_mem[readAddr(cpu)] = readByte(cpu);
        }
    });

    // nINT controller: assert every k_tickInterval cycles, hold for 2 machine cycles
    int machineCycles = 0;
    int intAssertedAt = -1;
    bool halted = false;
    bool lastWasS1 = false;

    cpu.pin(CDP1802::Terminal::TPA).connect<Level>([&](Level lvl) {
        if (lvl != Level::High)
        {
            return;
        }
        ++machineCycles;

        if (intAssertedAt >= 0 && machineCycles >= intAssertedAt + 2)
        {
            cpu.pin(CDP1802::Terminal::nINT).drive<Level>(Level::High);
            intAssertedAt = -1;
        }

        if (machineCycles % k_tickInterval == 0)
        {
            cpu.pin(CDP1802::Terminal::nINT).drive<Level>(Level::Low);
            intAssertedAt = machineCycles;
        }

        const bool s1 = cpu.pin(CDP1802::Terminal::SC1).read<Level>() == Level::Low &&
                        cpu.pin(CDP1802::Terminal::SC0).read<Level>() == Level::High;
        if (s1 && lastWasS1)
        {
            halted = true;
        }
        lastWasS1 = s1;
    });

    constexpr int k_cycleLimit = 1'000'000;
    int clocks = 0;
    for (; clocks < k_cycleLimit * 8 && !halted; ++clocks)
    {
        clockOnce(cpu);
    }

    const auto r5 = static_cast<int>(cpu.inspect("R5").value().value & 0xFFu);

    std::cout << "CDP1802 Interrupt Demo (Program 7)\n\n";
    std::cout << "  /INT interval : " << k_tickInterval << " cycles\n";
    std::cout << "  ticks per inc : " << static_cast<int>(k_targetTicks) << "\n";
    std::cout << "  target count  : " << static_cast<int>(k_maxCount) << "\n";
    std::cout << "  R5 (counter)  : " << r5 << "\n";
    std::cout << "  machine cycles: " << machineCycles << "\n";
    std::cout << "  status        : " << (halted ? "IDL (ok)" : "cycle limit hit") << "\n";

    return (r5 == k_maxCount && halted) ? 0 : 1;
}
