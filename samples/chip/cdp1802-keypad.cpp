/*
 * U1  CDP1802   CPU
 * U10 CDP1832   ROM 512x8, CS=MA7=Low (0x000..0x07F, page 0)
 * U4  I2114     SRAM low nibble,  CS=NOT(MA7) (0x080..0x0FF)
 * U5  I2114     SRAM high nibble, CS=NOT(MA7)
 * U6  SN74LS04  inverter: NOT(MA7) -> RAM /CS
 * U13 CD4515B   4-bit latch / 1-of-16 decoder (keypad column select)
 *
 * Keypad interface (VIP schematic):
 *   CPU OUT 2 (opcode 0x62) writes M(R(X)) onto BUS -> U13 DATA1-DATA4.
 *   U13 drives the selected column line Low (S0..S15).
 *   The pressed key shorts its column line to EF3 (active-Low).
 *   CPU reads EF3 via BN3 (0x3E, branch if nEF3=High) / B3 (branch if nEF3=Low).
 *
 * Keypad layout (physical position, layout-agnostic):
 *
 *   VIP key    Host key (physical position)
 *   1  2  3  C    1  2  3  4
 *   4  5  6  D    Q  W  E  R
 *   7  8  9  E    A  S  D  F
 *   A  0  B  F    Z  X  C  V
 *
 * ROM program: scans keys 0x0-0xF in a loop; when a key is detected,
 * stores its code at RAM[0x080] and loops back indefinitely.
 * Close the window to exit.
 */

#include <App.h>

#include <nfx/Silicon.h>

#include <array>
#include <cstdint>
#include <cstdio>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::chip::cdp1xxx;
using namespace nfx::silicon::decoder::cd4xxx;
using namespace nfx::silicon::memory::cdp1xxx;
using namespace nfx::silicon::memory::sram;
using namespace nfx::silicon::gate::sn74xxx;
using namespace nfx::silicon::samples;
using nfx::silicon::host::input::KeyCode;
using nfx::silicon::host::input::KeyEvent;

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
    constexpr CD4515B::Terminal k_s[] = {
        CD4515B::Terminal::S0,
        CD4515B::Terminal::S1,
        CD4515B::Terminal::S2,
        CD4515B::Terminal::S3,
        CD4515B::Terminal::S4,
        CD4515B::Terminal::S5,
        CD4515B::Terminal::S6,
        CD4515B::Terminal::S7,
        CD4515B::Terminal::S8,
        CD4515B::Terminal::S9,
        CD4515B::Terminal::S10,
        CD4515B::Terminal::S11,
        CD4515B::Terminal::S12,
        CD4515B::Terminal::S13,
        CD4515B::Terminal::S14,
        CD4515B::Terminal::S15
    };

    constexpr std::pair<KeyCode, uint8_t> k_keymap[] = {
        { KeyCode::Num1, uint8_t{ 0x1 } },
        { KeyCode::Num2, uint8_t{ 0x2 } },
        { KeyCode::Num3, uint8_t{ 0x3 } },
        { KeyCode::Num4, uint8_t{ 0xC } },
        { KeyCode::Q,    uint8_t{ 0x4 } },
        { KeyCode::W,    uint8_t{ 0x5 } },
        { KeyCode::E,    uint8_t{ 0x6 } },
        { KeyCode::R,    uint8_t{ 0xD } },
        { KeyCode::A,    uint8_t{ 0x7 } },
        { KeyCode::S,    uint8_t{ 0x8 } },
        { KeyCode::D,    uint8_t{ 0x9 } },
        { KeyCode::F,    uint8_t{ 0xE } },
        { KeyCode::Z,    uint8_t{ 0xA } },
        { KeyCode::X,    uint8_t{ 0x0 } },
        { KeyCode::C,    uint8_t{ 0xB } },
        { KeyCode::V,    uint8_t{ 0xF } }
    };
    // clang-format on

    auto vipKeyFromCode(const KeyCode code) -> int
    {
        for (const auto& [kc, vip] : k_keymap)
        {
            if (kc == code)
            {
                return static_cast<int>(vip);
            }
        }
        return -1;
    }

    // ROM: scan keypad 0x0..0xF, store pressed key at RAM[0x080], loop indefinitely
    auto buildRom() -> std::array<uint8_t, 512>
    {
        std::array<uint8_t, 512> r{};

        /*
         * Memory map (ROM page 0, 0x000-0x07F):
         *   0x000  program
         *   0x040  scan table: bytes 0x00..0x0F (OUT 2 reads M(R2) and increments R2)
         *   0x080  result cell in RAM (I2114, MA7=High)
         *
         * Registers:
         *   R0  PC (boot, then execution)
         *   R1  0x0080 result cell address
         *   R2  scan pointer (SEX R2, walks 0x040..0x04F then wraps)
         *   R3  key index counter (0..15)
         *
         * OUT 2: M(R(X)) -> BUS -> CD4515B DATA; R(X)++
         * BN3  : 0x3E, branch if nEF3=High (no key on scanned column)
         */

        constexpr uint16_t k_table = 0x040;
        for (uint8_t i = 0; i < 16; ++i)
        {
            r[k_table + i] = i;
        }

        uint16_t pc = 0x000;

        r[pc++] = 0xF8;
        r[pc++] = 0x80;
        r[pc++] = 0xA1; // LDI #80 / PLO R1
        r[pc++] = 0xF8;
        r[pc++] = 0x00;
        r[pc++] = 0xB1; // LDI #00 / PHI R1

        r[pc++] = 0xF8;
        r[pc++] = static_cast<uint8_t>(k_table);
        r[pc++] = 0xA2;
        r[pc++] = 0xF8;
        r[pc++] = 0x00;
        r[pc++] = 0xB2;
        r[pc++] = 0xE2; // SEX R2

        r[pc++] = 0xF8;
        r[pc++] = 0x00;
        r[pc++] = 0xA3; // PLO R3
        r[pc++] = 0xB3; // PHI R3

        const uint8_t addr_scan = static_cast<uint8_t>(pc);
        r[pc++] = 0x62; // OUT 2

        r[pc++] = 0x3E;
        const uint16_t patch_bn3 = pc;
        r[pc++] = 0x00;

        r[pc++] = 0x83; // GLO R3
        r[pc++] = 0x51; // STR R1

        r[pc++] = 0xF8;
        r[pc++] = static_cast<uint8_t>(k_table);
        r[pc++] = 0xA2;
        r[pc++] = 0xF8;
        r[pc++] = 0x00;
        r[pc++] = 0xA3;
        r[pc++] = 0x30;
        r[pc++] = addr_scan;

        const uint8_t addr_next = static_cast<uint8_t>(pc);
        r[patch_bn3] = addr_next;
        r[pc++] = 0x13; // INC R3
        r[pc++] = 0x83; // GLO R3
        r[pc++] = 0xFF;
        r[pc++] = 0x10; // XRI #10
        r[pc++] = 0x3A;
        const uint16_t patch_bnz = pc;
        r[pc++] = 0x00;

        r[pc++] = 0xF8;
        r[pc++] = static_cast<uint8_t>(k_table);
        r[pc++] = 0xA2;
        r[pc++] = 0xF8;
        r[pc++] = 0x00;
        r[pc++] = 0xA3;
        r[pc++] = 0x30;
        r[pc++] = addr_scan;

        r[patch_bnz] = addr_scan;

        return r;
    }

    auto preloadRam(I2114& lo, I2114& hi) -> void
    {
        for (uint16_t addr = 0; addr < 256; ++addr)
        {
            for (int b = 0; b < 8; ++b)
            {
                const Level lv = ((addr >> b) & 1u) ? Level::High : Level::Low;
                lo.pin(k_ram_a[b]).drive<Level>(lv);
                hi.pin(k_ram_a[b]).drive<Level>(lv);
            }
            lo.pin(I2114::Terminal::A8).drive<Level>(Level::Low);
            lo.pin(I2114::Terminal::A9).drive<Level>(Level::Low);
            hi.pin(I2114::Terminal::A8).drive<Level>(Level::Low);
            hi.pin(I2114::Terminal::A9).drive<Level>(Level::Low);
            lo.pin(I2114::Terminal::CS).drive<Level>(Level::Low);
            hi.pin(I2114::Terminal::CS).drive<Level>(Level::Low);
            for (int b = 0; b < 4; ++b)
            {
                lo.pin(k_lo_io[b]).drive<Level>(Level::Low);
                hi.pin(k_lo_io[b]).drive<Level>(Level::Low);
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
        lo.pin(I2114::Terminal::A9).drive<Level>(Level::Low);
        hi.pin(I2114::Terminal::A8).drive<Level>(Level::Low);
        hi.pin(I2114::Terminal::A9).drive<Level>(Level::Low);
        lo.pin(I2114::Terminal::CS).release();
        hi.pin(I2114::Terminal::CS).release();
        lo.pin(I2114::Terminal::WE).release();
        hi.pin(I2114::Terminal::WE).release();
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

    CDP1802 cpu{ CDP1802::Descriptor{ "U1" } };
    CDP1832 rom{ CDP1832::Descriptor{ "U10", romData } };
    I2114 ramLo{ I2114::Descriptor{ "U4" } };
    I2114 ramHi{ I2114::Descriptor{ "U5" } };
    LS04 inv{ LS04::Descriptor{ "U6" } };
    CD4515B dec{ CD4515B::Descriptor{ "U13" } };

    ramLo.pin(I2114::Terminal::VCC).drive<float>(5.0f);
    ramHi.pin(I2114::Terminal::VCC).drive<float>(5.0f);
    preloadRam(ramLo, ramHi);

    rom.pin(CDP1832::Terminal::VDD).drive<float>(5.0f);
    rom.pin(CDP1832::Terminal::A8).drive<Level>(Level::Low);

    dec.pin(CD4515B::Terminal::VDD).drive<float>(5.0f);
    dec.pin(CD4515B::Terminal::INHIBIT).drive<Level>(Level::Low);
    dec.pin(CD4515B::Terminal::STROBE).drive<Level>(Level::High);

    inv.pin(LS04::Terminal::VCC).drive<float>(5.0f);

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
        wMA[i]->attach(rom.pin(k_rom_ma[i]));
        wMA[i]->attach(ramLo.pin(k_ram_a[i]));
        wMA[i]->attach(ramHi.pin(k_ram_a[i]));
    }

    wMA7.attach(rom.pin(CDP1832::Terminal::CS));
    wMA7.attach(inv.pin(LS04::Terminal::A1));

    Wire wRamCS{ Wire::Descriptor{ .name = "wRamCS", .kind = Pin::Kind::Digital } };
    wRamCS.attach(inv.pin(LS04::Terminal::Y1));
    wRamCS.attach(ramLo.pin(I2114::Terminal::CS));
    wRamCS.attach(ramHi.pin(I2114::Terminal::CS));

    Wire wWE{ Wire::Descriptor{ .name = "wWE", .kind = Pin::Kind::Digital } };
    wWE.attach(cpu.pin(CDP1802::Terminal::nMWR));
    wWE.attach(ramLo.pin(I2114::Terminal::WE));
    wWE.attach(ramHi.pin(I2114::Terminal::WE));

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

    // OUT 2 puts M(R(X)) on BUS during TPB; BUS0-3 feed CD4515B DATA1-4 (keypad column select)
    wBUS[0]->attach(dec.pin(CD4515B::Terminal::DATA1));
    wBUS[1]->attach(dec.pin(CD4515B::Terminal::DATA2));
    wBUS[2]->attach(dec.pin(CD4515B::Terminal::DATA3));
    wBUS[3]->attach(dec.pin(CD4515B::Terminal::DATA4));

    int pressedVipKey = -1;

    cpu.pin(CDP1802::Terminal::nEF3).connect<Level>([](Level) {});

    auto updateEF3 = [&]() {
        int scanned = 0;
        for (int i = 0; i < 16; ++i)
        {
            if (dec.pin(k_s[i]).read<Level>() == Level::Low)
            {
                scanned = i;
                break;
            }
        }
        const Level ef3 = (pressedVipKey >= 0 && pressedVipKey == scanned) ? Level::Low : Level::High;
        cpu.pin(CDP1802::Terminal::nEF3).drive<Level>(ef3);
    };

    for (int i = 0; i < 16; ++i)
    {
        dec.pin(k_s[i]).connect<Level>([&](Level) { updateEF3(); });
    }

    cpu.pin(CDP1802::Terminal::VDD).drive<float>(5.0f);
    cpu.pin(CDP1802::Terminal::nCLEAR).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::nWAIT).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::CLOCK).drive<Level>(Level::Low);
    cpu.pin(CDP1802::Terminal::nEF1).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::nEF2).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::nEF3).drive<Level>(Level::High);
    cpu.pin(CDP1802::Terminal::nEF4).drive<Level>(Level::High);

    std::printf("CDP1802 + CDP1832 + I2114 + CD4515B - hex keypad\n\n");
    std::printf("VIP keypad layout:\n");
    std::printf("  VIP: 1 2 3 C    keys: 1 2 3 4    1 2 3 4\n");
    std::printf("       4 5 6 D          Q W E R    A Z E R\n");
    std::printf("       7 8 9 E          A S D F    Q S D F\n");
    std::printf("       A 0 B F          Z X C V    W X C V\n\n");
    std::printf("Press a key. Close the window to exit.\n\n");

    App app(App::Config{
        .title = "nfx::silicon CDP1802 hex keypad",
        .width = 320,
        .height = 240,
        .glMajor = 3,
        .glMinor = 3,
    });

    constexpr int k_clocksPerFrame = 512;

    return app.run(
        nullptr,
        nullptr,
        [&](const int, const int) {
            for (int i = 0; i < k_clocksPerFrame; ++i)
            {
                tick(cpu);
            }
        },
        [&](const KeyEvent& ev) {
            const int vip = vipKeyFromCode(ev.key);
            if (vip < 0)
            {
                return;
            }
            if (ev.pressed)
            {
                pressedVipKey = vip;
                updateEF3();
                std::printf("Key pressed: VIP=0x%X\n", vip);
                std::fflush(stdout);
            }
            else
            {
                pressedVipKey = -1;
                updateEF3();
            }
        });
}
