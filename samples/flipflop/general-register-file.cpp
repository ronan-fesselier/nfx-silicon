#include <nfx/Silicon.h>

#include <iostream>

using namespace nfx::silicon;
using namespace nfx::silicon::decoder::sn74xxx;
using namespace nfx::silicon::flipflop::sn74xxx;
using namespace nfx::silicon::signal;

// ---------------------------------------------------------------------------
// Expandable 4-Word by 8-Bit General Register File
// SN74LS374 datasheet typical application
//
// Four 8-bit registers share a common 3-STATE data bus.
//
// 1/2 LS139 (enable decoder): /1G=Low, A/B=EnableSelect -> Y0..Y3 -> OC of each LS374
// 1/2 LS139 (clock decoder):  /2G=Clock, A/B=ClockSelect -> Y0..Y3 -> CLK of each LS374
//
// OC is active-Low: Y=Low enables register output onto the bus.
// CLK samples on rising edge: /2G=Low->High pulses Y Low->High = rising edge on CLK.
// ---------------------------------------------------------------------------

static void printBus(const char* label, const Wire* bus[8])
{
    std::cout << label << ' ';
    for (int i = 7; i >= 0; --i)
    {
        const Level l = bus[i]->read<Level>();
        std::cout << (l == Level::High ? '1' : l == Level::Low ? '0' : '-');
    }
    std::cout << '\n';
}

int main()
{
    // LS139 package: decoder 1 = enable, decoder 2 = clock
    LS139 dec{ { .name = "U1" } };
    LS374 reg0{ { .name = "R0" } };
    LS374 reg1{ { .name = "R1" } };
    LS374 reg2{ { .name = "R2" } };
    LS374 reg3{ { .name = "R3" } };

    dec.pin("VCC").drive<float>(5.0f);
    reg0.pin("VCC").drive<float>(5.0f);
    reg1.pin("VCC").drive<float>(5.0f);
    reg2.pin("VCC").drive<float>(5.0f);
    reg3.pin("VCC").drive<float>(5.0f);

    dec.pin("/1G").drive<Level>(Level::Low);

    dec.pin("1Y0").connect<Level>([&](const Level l) { reg0.pin("OC").drive<Level>(l); });
    dec.pin("1Y1").connect<Level>([&](const Level l) { reg1.pin("OC").drive<Level>(l); });
    dec.pin("1Y2").connect<Level>([&](const Level l) { reg2.pin("OC").drive<Level>(l); });
    dec.pin("1Y3").connect<Level>([&](const Level l) { reg3.pin("OC").drive<Level>(l); });

    dec.pin("2Y0").connect<Level>([&](const Level l) { reg0.pin("CLK").drive<Level>(l); });
    dec.pin("2Y1").connect<Level>([&](const Level l) { reg1.pin("CLK").drive<Level>(l); });
    dec.pin("2Y2").connect<Level>([&](const Level l) { reg2.pin("CLK").drive<Level>(l); });
    dec.pin("2Y3").connect<Level>([&](const Level l) { reg3.pin("CLK").drive<Level>(l); });

    // 3-STATE data bus: WiredAnd so idle Q pins (OC=High -> HighZ) do not fight
    const char* dPins[8] = { "1D", "2D", "3D", "4D", "5D", "6D", "7D", "8D" };
    const char* qPins[8] = { "1Q", "2Q", "3Q", "4Q", "5Q", "6Q", "7Q", "8Q" };

    Wire* busWires[8];
    Wire w0{ { .name = "D0", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };
    Wire w1{ { .name = "D1", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };
    Wire w2{ { .name = "D2", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };
    Wire w3{ { .name = "D3", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };
    Wire w4{ { .name = "D4", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };
    Wire w5{ { .name = "D5", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };
    Wire w6{ { .name = "D6", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };
    Wire w7{ { .name = "D7", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };
    busWires[0] = &w0;
    busWires[1] = &w1;
    busWires[2] = &w2;
    busWires[3] = &w3;
    busWires[4] = &w4;
    busWires[5] = &w5;
    busWires[6] = &w6;
    busWires[7] = &w7;

    LS374* regs[4] = { &reg0, &reg1, &reg2, &reg3 };
    for (auto* r : regs)
    {
        for (int i = 0; i < 8; ++i)
        {
            busWires[i]->attach(r->pin(dPins[i]));
            busWires[i]->attach(r->pin(qPins[i]));
        }
    }

    // External data source pins (simulates external driver on the bus)
    Pin src[8] = { Pin{ { .name = "SRC0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                   Pin{ { .name = "SRC1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                   Pin{ { .name = "SRC2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                   Pin{ { .name = "SRC3", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                   Pin{ { .name = "SRC4", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                   Pin{ { .name = "SRC5", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                   Pin{ { .name = "SRC6", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                   Pin{ { .name = "SRC7", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } } };
    for (int i = 0; i < 8; ++i)
    {
        busWires[i]->attach(src[i]);
    }

    auto driveData = [&](const std::uint8_t val) {
        for (int i = 0; i < 8; ++i)
        {
            src[i].drive<Level>((val >> i) & 1u ? Level::High : Level::Low);
        }
    };

    auto releaseData = [&]() {
        for (int i = 0; i < 8; ++i)
        {
            src[i].release();
        }
    };

    auto selectEnable = [&](const std::uint8_t reg) {
        dec.pin("1A").drive<Level>(reg & 1u ? Level::High : Level::Low);
        dec.pin("1B").drive<Level>(reg & 2u ? Level::High : Level::Low);
    };

    auto clockWrite = [&](const std::uint8_t reg) {
        dec.pin("/1G").drive<Level>(Level::High); // isolate OC during write to avoid bus contention
        dec.pin("2A").drive<Level>(reg & 1u ? Level::High : Level::Low);
        dec.pin("2B").drive<Level>(reg & 2u ? Level::High : Level::Low);
        dec.pin("/2G").drive<Level>(Level::Low); // Low->High on /2G = rising edge on selected CLK
        dec.pin("/2G").drive<Level>(Level::High);
        dec.pin("/1G").drive<Level>(Level::Low);
    };

    const Wire* busView[8];
    for (int i = 0; i < 8; ++i)
    {
        busView[i] = busWires[i];
    }

    std::cout << "4-Word by 8-Bit General Register File\n";
    std::cout << "SN74LS374 datasheet application\n\n";

    dec.pin("/1G").drive<Level>(Level::High);
    dec.pin("1A").drive<Level>(Level::Low);
    dec.pin("1B").drive<Level>(Level::Low);
    dec.pin("2A").drive<Level>(Level::Low);
    dec.pin("2B").drive<Level>(Level::Low);
    dec.pin("/2G").drive<Level>(Level::High);

    driveData(0xA5);
    clockWrite(0);
    releaseData();
    std::cout << "Write 0xA5 -> R0\n";

    driveData(0x3C);
    clockWrite(1);
    releaseData();
    std::cout << "Write 0x3C -> R1\n";

    driveData(0xF0);
    clockWrite(2);
    releaseData();
    std::cout << "Write 0xF0 -> R2\n";

    driveData(0x0F);
    clockWrite(3);
    releaseData();
    std::cout << "Write 0x0F -> R3\n\n";

    selectEnable(0);
    printBus("R0:", busView);

    selectEnable(1);
    printBus("R1:", busView);

    selectEnable(2);
    printBus("R2:", busView);

    selectEnable(3);
    printBus("R3:", busView);
}
