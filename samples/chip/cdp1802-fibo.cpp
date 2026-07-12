/*
 * U1 CDP1802    CPU
 * U2 SN74LS373  address latch (TPA clocks high byte, unused for decode here)
 * U3 CDP1832    ROM 128x8, CS=MA7 (active Low on 0x000..0x07F)
 * U4 I2114      SRAM low nibble,  CS=NOT(MA7) (active Low on 0x080..0x0FF)
 * U5 I2114      SRAM high nibble, CS=NOT(MA7)
 * U6 SN74LS04   inverter: NOT(MA7) drives RAM /CS
 *
 * Single page (A8=Low throughout):
 *   0x000..0x07F  ROM  (MA7=Low  -> ROM CS=Low,  RAM CS=High)
 *   0x080..0x0FF  RAM  (MA7=High -> ROM CS=High, RAM CS=Low)
 *
 * ROM layout (16-bit Fibonacci, low byte first then high byte, ADD/ADC carry chain):
 *   0x000: init R2=0x0080 (a_lo), R3=0x0082 (b_lo), R4=0x0084 (scratch_lo), SEX R2
 *   loop:  LDN R3, STR R4, ADD, STR R3   (low byte, DF=carry)
 *          INC R2, INC R3, INC R4
 *          LDN R3, STR R4, ADC, BDF halt (high byte, DF=overflow)
 *          STR R3, LDN R4, STR R2
 *          DEC R4, DEC R3, DEC R2
 *          LDN R4, STR R2, BR loop
 *
 * RAM layout:
 *   0x080: a_lo = F(0) low byte  = 0x00
 *   0x081: a_hi = F(0) high byte = 0x00
 *   0x082: b_lo = F(1) low byte  = 0x01
 *   0x083: b_hi = F(1) high byte = 0x00
 *   0x084: scratch_lo
 *   0x085: scratch_hi
 *
 * Each write to 0x082/0x083 (new b, low then high byte) is captured and
 * combined into a 16-bit term. Overflow (DF=1 after the high-byte ADC)
 * branches to IDL and halts (F(24)=46368 is the last term that fits in 16 bits).
 */

#include <nfx/Silicon.h>

#include <cstdint>
#include <iostream>
#include <vector>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::chip::cdp1xxx;
using namespace nfx::silicon::latch::sn74xxx;
using namespace nfx::silicon::memory::cdp1xxx;
using namespace nfx::silicon::memory::sram;
using namespace nfx::silicon::gate::sn74xxx;

namespace
{
    // clang-format off
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
    constexpr LS373::Terminal k_latch_d[] = {
        LS373::Terminal::D1,
        LS373::Terminal::D2,
        LS373::Terminal::D3,
        LS373::Terminal::D4,
        LS373::Terminal::D5,
        LS373::Terminal::D6,
        LS373::Terminal::D7,
        LS373::Terminal::D8
    };
    constexpr CDP1832::Terminal k_rom_ma[] = {
        CDP1832::Terminal::MA0,
        CDP1832::Terminal::MA1,
        CDP1832::Terminal::MA2,
        CDP1832::Terminal::MA3,
        CDP1832::Terminal::MA4,
        CDP1832::Terminal::MA5,
        CDP1832::Terminal::MA6,
        CDP1832::Terminal::MA7
    };
    constexpr CDP1832::Terminal k_rom_bus[] = {
        CDP1832::Terminal::BUS0,
        CDP1832::Terminal::BUS1,
        CDP1832::Terminal::BUS2,
        CDP1832::Terminal::BUS3,
        CDP1832::Terminal::BUS4,
        CDP1832::Terminal::BUS5,
        CDP1832::Terminal::BUS6,
        CDP1832::Terminal::BUS7
    };

    constexpr I2114::Terminal k_ram_a[] = {
        I2114::Terminal::A0,
        I2114::Terminal::A1,
        I2114::Terminal::A2,
        I2114::Terminal::A3,
        I2114::Terminal::A4,
        I2114::Terminal::A5,
        I2114::Terminal::A6,
        I2114::Terminal::A7
    };
    constexpr I2114::Terminal k_lo_io[] = {
        I2114::Terminal::IO1,
        I2114::Terminal::IO2,
        I2114::Terminal::IO3,
        I2114::Terminal::IO4
    };
    // clang-format on

    auto buildRom() -> std::array<uint8_t, 512>
    {
        std::array<uint8_t, 512> r{};

        // clang-format off
        r[0x000] = 0xF8; r[0x001] = 0x80; // LDI #$80
        r[0x002] = 0xA2;                  // PLO R2
        r[0x003] = 0xF8; r[0x004] = 0x00; // LDI #$00
        r[0x005] = 0xB2;                  // PHI R2  -> R2=0x0080 (a_lo)
        r[0x006] = 0xF8; r[0x007] = 0x82; // LDI #$82
        r[0x008] = 0xA3;                  // PLO R3
        r[0x009] = 0xF8; r[0x00A] = 0x00; // LDI #$00
        r[0x00B] = 0xB3;                  // PHI R3  -> R3=0x0082 (b_lo)
        r[0x00C] = 0xF8; r[0x00D] = 0x84; // LDI #$84
        r[0x00E] = 0xA4;                  // PLO R4
        r[0x00F] = 0xF8; r[0x010] = 0x00; // LDI #$00
        r[0x011] = 0xB4;                  // PHI R4  -> R4=0x0084 (scratch_lo)
        r[0x012] = 0xE2;                  // SEX R2  -> X=2

        // Low byte: b_lo = a_lo + b_lo, DF = carry
        r[0x013] = 0x03;                  // LDN R3  -> D=b_lo
        r[0x014] = 0x54;                  // STR R4  -> scratch_lo=b_lo
        r[0x015] = 0xF4;                  // ADD     -> D=a_lo+b_lo, DF=carry
        r[0x016] = 0x53;                  // STR R3  -> b_lo=new low byte

        r[0x017] = 0x12;                  // INC R2  -> a_hi
        r[0x018] = 0x13;                  // INC R3  -> b_hi
        r[0x019] = 0x14;                  // INC R4  -> scratch_hi

        // High byte: b_hi = a_hi + b_hi + carry, DF = 16-bit overflow
        r[0x01A] = 0x03;                  // LDN R3  -> D=b_hi
        r[0x01B] = 0x54;                  // STR R4  -> scratch_hi=b_hi
        r[0x01C] = 0x74;                  // ADC     -> D=a_hi+b_hi+DF, DF=overflow
        r[0x01D] = 0x33; r[0x01E] = 0x2A; // BDF 0x02A (halt on 16-bit overflow)
        r[0x01F] = 0x53;                  // STR R3  -> b_hi=new high byte
        r[0x020] = 0x04;                  // LDN R4  -> D=scratch_hi=old b_hi
        r[0x021] = 0x52;                  // STR R2  -> a_hi=old b_hi

        r[0x022] = 0x24;                  // DEC R4  -> scratch_lo
        r[0x023] = 0x23;                  // DEC R3  -> b_lo
        r[0x024] = 0x22;                  // DEC R2  -> a_lo

        r[0x025] = 0x04;                  // LDN R4  -> D=scratch_lo=old b_lo
        r[0x026] = 0x52;                  // STR R2  -> a_lo=old b_lo
        r[0x027] = 0x30; r[0x028] = 0x13; // BR 0x013

        r[0x02A] = 0x00;                  // IDL
        // clang-format on

        return r;
    }

    auto buildRam() -> std::array<uint8_t, 256>
    {
        std::array<uint8_t, 256> m{};
        m[0x080] = 0x00; // a_lo = F(0) low byte
        m[0x081] = 0x00; // a_hi = F(0) high byte
        m[0x082] = 0x01; // b_lo = F(1) low byte
        m[0x083] = 0x00; // b_hi = F(1) high byte
        m[0x084] = 0x00; // scratch_lo
        m[0x085] = 0x00; // scratch_hi
        return m;
    }

    auto preloadRam(I2114& lo, I2114& hi, const std::array<uint8_t, 256>& mem) -> void
    {
        for (uint16_t addr = 0; addr < 256; ++addr)
        {
            const uint8_t byte = mem[addr];
            const uint8_t nlo = static_cast<uint8_t>(byte & 0x0Fu);
            const uint8_t nhi = static_cast<uint8_t>((byte >> 4) & 0x0Fu);

            for (int b = 0; b < 8; ++b)
            {
                const Level lv = ((addr >> b) & 1u) ? Level::High : Level::Low;
                lo.pin(k_ram_a[b]).drive<Level>(lv);
                hi.pin(k_ram_a[b]).drive<Level>(lv);
            }
            lo.pin(I2114::Terminal::A8).drive<Level>(Level::Low);
            hi.pin(I2114::Terminal::A8).drive<Level>(Level::Low);
            lo.pin(I2114::Terminal::A9).drive<Level>(Level::Low);
            hi.pin(I2114::Terminal::A9).drive<Level>(Level::Low);

            lo.pin(I2114::Terminal::CS).drive<Level>(Level::Low);
            hi.pin(I2114::Terminal::CS).drive<Level>(Level::Low);

            for (int b = 0; b < 4; ++b)
            {
                lo.pin(k_lo_io[b]).drive<Level>(((nlo >> b) & 1u) ? Level::High : Level::Low);
                hi.pin(k_lo_io[b]).drive<Level>(((nhi >> b) & 1u) ? Level::High : Level::Low);
            }

            lo.pin(I2114::Terminal::WE).drive<Level>(Level::Low);
            hi.pin(I2114::Terminal::WE).drive<Level>(Level::Low);
            lo.pin(I2114::Terminal::WE).drive<Level>(Level::High);
            hi.pin(I2114::Terminal::WE).drive<Level>(Level::High);

            lo.pin(I2114::Terminal::CS).drive<Level>(Level::High);
            hi.pin(I2114::Terminal::CS).drive<Level>(Level::High);
        }

        for (const auto t : k_lo_io)
        {
            lo.pin(t).release();
            hi.pin(t).release();
        }
        for (const auto t : k_ram_a)
        {
            lo.pin(t).release();
            hi.pin(t).release();
        }
        lo.pin(I2114::Terminal::A8).drive<Level>(Level::Low);
        hi.pin(I2114::Terminal::A8).drive<Level>(Level::Low);
        lo.pin(I2114::Terminal::A9).drive<Level>(Level::Low);
        hi.pin(I2114::Terminal::A9).drive<Level>(Level::Low);
        lo.pin(I2114::Terminal::CS).release();
        hi.pin(I2114::Terminal::CS).release();
        lo.pin(I2114::Terminal::WE).release();
        hi.pin(I2114::Terminal::WE).release();
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

    auto tick(CDP1802& cpu) -> void
    {
        cpu.pin(CDP1802::Terminal::CLOCK).drive<Level>(Level::High);
        cpu.pin(CDP1802::Terminal::CLOCK).drive<Level>(Level::Low);
    }
} // namespace

int main()
{
    const auto romData = buildRom();
    const auto ramInit = buildRam();

    CDP1802 cpu{ CDP1802::Descriptor{ "U1" } };
    LS373 latch{ LS373::Descriptor{ "U2" } };
    CDP1832 rom{ CDP1832::Descriptor{ "U3", romData } };
    I2114 ramLo{ I2114::Descriptor{ "U4" } };
    I2114 ramHi{ I2114::Descriptor{ "U5" } };
    LS04 inv{ LS04::Descriptor{ "U6" } };

    ramLo.pin(I2114::Terminal::VCC).drive<float>(5.0f);
    ramHi.pin(I2114::Terminal::VCC).drive<float>(5.0f);

    preloadRam(ramLo, ramHi, ramInit);

    // Address bus: CPU MA -> latch, ROM, RAM
    Wire wMA0{ Wire::Descriptor{ .name = "wMA0", .kind = Pin::Kind::Digital } };
    Wire wMA1{ Wire::Descriptor{ .name = "wMA1", .kind = Pin::Kind::Digital } };
    Wire wMA2{ Wire::Descriptor{ .name = "wMA2", .kind = Pin::Kind::Digital } };
    Wire wMA3{ Wire::Descriptor{ .name = "wMA3", .kind = Pin::Kind::Digital } };
    Wire wMA4{ Wire::Descriptor{ .name = "wMA4", .kind = Pin::Kind::Digital } };
    Wire wMA5{ Wire::Descriptor{ .name = "wMA5", .kind = Pin::Kind::Digital } };
    Wire wMA6{ Wire::Descriptor{ .name = "wMA6", .kind = Pin::Kind::Digital } };
    Wire wMA7{ Wire::Descriptor{ .name = "wMA7", .kind = Pin::Kind::Digital } };

    Wire* wMA[8] = { &wMA0, &wMA1, &wMA2, &wMA3, &wMA4, &wMA5, &wMA6, &wMA7 };
    for (int i = 0; i < 8; ++i)
    {
        wMA[i]->attach(cpu.pin(k_ma[i]));
        wMA[i]->attach(latch.pin(k_latch_d[i]));
        wMA[i]->attach(rom.pin(k_rom_ma[i]));
        wMA[i]->attach(ramLo.pin(k_ram_a[i]));
        wMA[i]->attach(ramHi.pin(k_ram_a[i]));
    }

    // TPA clocks the LS373 latch (captures high address byte)
    Wire wTPA{ Wire::Descriptor{ .name = "wTPA", .kind = Pin::Kind::Digital } };
    wTPA.attach(cpu.pin(CDP1802::Terminal::TPA));
    wTPA.attach(latch.pin(LS373::Terminal::C));

    // MA7=Low  -> ROM CS=Low  (0x000..0x07F)
    // MA7=High -> inv.Y1=Low -> RAM CS=Low (0x080..0x0FF)
    wMA7.attach(rom.pin(CDP1832::Terminal::CS));
    wMA7.attach(inv.pin(LS04::Terminal::A1));

    Wire wRamCS{ Wire::Descriptor{ .name = "wRamCS", .kind = Pin::Kind::Digital } };
    wRamCS.attach(inv.pin(LS04::Terminal::Y1));
    wRamCS.attach(ramLo.pin(I2114::Terminal::CS));
    wRamCS.attach(ramHi.pin(I2114::Terminal::CS));

    // nMWR drives RAM /WE
    Wire wWE{ Wire::Descriptor{ .name = "wWE", .kind = Pin::Kind::Digital } };
    wWE.attach(cpu.pin(CDP1802::Terminal::nMWR));
    wWE.attach(ramLo.pin(I2114::Terminal::WE));
    wWE.attach(ramHi.pin(I2114::Terminal::WE));

    // Data bus: CPU BUS <-> ROM BUS, low nibble -> ramLo IO, high nibble -> ramHi IO
    Wire wBUS0{ Wire::Descriptor{ .name = "wBUS0", .kind = Pin::Kind::Digital } };
    Wire wBUS1{ Wire::Descriptor{ .name = "wBUS1", .kind = Pin::Kind::Digital } };
    Wire wBUS2{ Wire::Descriptor{ .name = "wBUS2", .kind = Pin::Kind::Digital } };
    Wire wBUS3{ Wire::Descriptor{ .name = "wBUS3", .kind = Pin::Kind::Digital } };
    Wire wBUS4{ Wire::Descriptor{ .name = "wBUS4", .kind = Pin::Kind::Digital } };
    Wire wBUS5{ Wire::Descriptor{ .name = "wBUS5", .kind = Pin::Kind::Digital } };
    Wire wBUS6{ Wire::Descriptor{ .name = "wBUS6", .kind = Pin::Kind::Digital } };
    Wire wBUS7{ Wire::Descriptor{ .name = "wBUS7", .kind = Pin::Kind::Digital } };

    Wire* wBUS[8] = { &wBUS0, &wBUS1, &wBUS2, &wBUS3, &wBUS4, &wBUS5, &wBUS6, &wBUS7 };
    for (int i = 0; i < 8; ++i)
    {
        wBUS[i]->attach(cpu.pin(k_bus[i]));
        wBUS[i]->attach(rom.pin(k_rom_bus[i]));
    }
    for (int i = 0; i < 4; ++i)
    {
        wBUS[i]->attach(ramLo.pin(k_lo_io[i]));
        wBUS[i + 4]->attach(ramHi.pin(k_lo_io[i]));
    }

    // nMWR: capture b_lo at 0x082, then b_hi at 0x083, combine into 16-bit term
    std::vector<uint16_t> sequence;
    sequence.push_back(0x0000);
    sequence.push_back(0x0001);

    uint8_t pendingLo = 0;
    cpu.pin(CDP1802::Terminal::nMWR).connect<Level>([&](const Level lvl) {
        if (lvl != Level::Low)
        {
            return;
        }
        const auto addr = readAddr(cpu);
        if (addr == 0x82)
        {
            pendingLo = readByte(cpu);
            return;
        }
        if (addr == 0x83)
        {
            const uint8_t hi = readByte(cpu);
            sequence.push_back(static_cast<uint16_t>((hi << 8) | pendingLo));
        }
    });

    latch.pin(LS373::Terminal::VCC).drive<float>(5.0f);
    latch.pin(LS373::Terminal::OC).drive<Level>(Level::Low);

    inv.pin(LS04::Terminal::VCC).drive<float>(5.0f);

    rom.pin(CDP1832::Terminal::VDD).drive<float>(5.0f);
    rom.pin(CDP1832::Terminal::A8).drive<Level>(Level::Low);

    cpu.pin(CDP1802::Terminal::VDD).drive<float>(5.0f);
    cpu.pin(CDP1802::Terminal::nCLEAR).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::nWAIT).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::CLOCK).drive<Level>(Level::Low);

    bool halted = false;
    bool lastWasS1 = false;

    cpu.pin(CDP1802::Terminal::TPA).connect<Level>([&](const Level lvl) {
        if (lvl != Level::High)
        {
            return;
        }
        const bool s1 = cpu.pin(CDP1802::Terminal::SC1).read<Level>() == Level::Low &&
                        cpu.pin(CDP1802::Terminal::SC0).read<Level>() == Level::High;
        if (s1 && lastWasS1)
        {
            halted = true;
        }
        lastWasS1 = s1;
    });

    constexpr int k_cycleLimit = 100000;
    int cycles = 0;
    for (; cycles < k_cycleLimit && !halted; ++cycles)
    {
        tick(cpu);
    }

    std::cout << "CDP1802 + LS373 + CDP1832 + I2114x2 + LS04\n";
    std::cout << "Fibonacci sequence (16-bit):\n\n";
    for (std::size_t i = 0; i < sequence.size(); ++i)
    {
        std::cout << "  F(" << i << ") = " << sequence[i] << '\n';
    }
    std::cout << '\n' << sequence.size() << " terms before 16-bit overflow.\n";
    std::cout << cycles << " clock cycles" << (halted ? " (IDL)\n" : " (cycle limit reached)\n");

    return 0;
}
