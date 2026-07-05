#include <nfx/Silicon.h>

#include <iostream>

using namespace nfx::silicon;
using namespace nfx::silicon::flipflop::sn74xxx;
using namespace nfx::silicon::gate::sn74xxx;
using namespace nfx::silicon::signal;

// ---------------------------------------------------------------------------
// Bidirectional Bus Driver (SN74LS374 datasheet typical application)
//
// Two LS374 octal D flip-flops transfer data between two 8-bit buses.
//
//   Bus1 -> LS374_A (clocked by Clock1 & BusExchangeClock) -> Bus2
//   Bus2 -> LS374_B (clocked by Clock2 & BusExchangeClock) -> Bus1
//
// Output control (active-Low OC pins) is inverted via LS04:
//   OC_A = NOT(OutputControl1)
//   OC_B = NOT(OutputControl2)
//
// Clock gating via LS08:
//   CLK_A = Clock1 & BusExchangeClock
//   CLK_B = Clock2 & BusExchangeClock
// ---------------------------------------------------------------------------

int main()
{
    // Packages
    LS374 regA{ { .name = "U1" } }; // Bus1 -> Bus2
    LS374 regB{ { .name = "U2" } }; // Bus2 -> Bus1
    LS08 clkGate{ { .name = "U3" } };
    LS04 ocInv{ { .name = "U4" } };

    // Power all packages
    regA.pin("VCC").drive<float>(5.0f);
    regB.pin("VCC").drive<float>(5.0f);
    clkGate.pin("VCC").drive<float>(5.0f);
    ocInv.pin("VCC").drive<float>(5.0f);

    // Control signals
    Pin pinClock1{ { .name = "Clock1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
    Pin pinClock2{ { .name = "Clock2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
    Pin pinBEC{ { .name = "BusExchangeClock", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
    Pin pinOC1{ { .name = "OutputControl1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
    Pin pinOC2{ { .name = "OutputControl2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

    // Clock gating: CLK_A = Clock1 & BusExchangeClock (gate 1)
    //               CLK_B = Clock2 & BusExchangeClock (gate 2)
    pinClock1.connect<Level>([&](const Level l) { clkGate.pin("1A").drive<Level>(l); });
    pinBEC.connect<Level>([&](const Level l) {
        clkGate.pin("1B").drive<Level>(l);
        clkGate.pin("2B").drive<Level>(l);
    });
    pinClock2.connect<Level>([&](const Level l) { clkGate.pin("2A").drive<Level>(l); });

    clkGate.pin("1Y").connect<Level>([&](const Level l) { regA.pin("CLK").drive<Level>(l); });
    clkGate.pin("2Y").connect<Level>([&](const Level l) { regB.pin("CLK").drive<Level>(l); });

    // Output control inversion via LS04
    pinOC1.connect<Level>([&](const Level l) { ocInv.pin("1A").drive<Level>(l); });
    pinOC2.connect<Level>([&](const Level l) { ocInv.pin("2A").drive<Level>(l); });

    ocInv.pin("1Y").connect<Level>([&](const Level l) { regA.pin("OC").drive<Level>(l); });
    ocInv.pin("2Y").connect<Level>([&](const Level l) { regB.pin("OC").drive<Level>(l); });

    // Bus1 data pins (8 bits)
    Pin bus1[8] = { Pin{ { .name = "Bus1_1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus1_2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus1_3", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus1_4", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus1_5", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus1_6", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus1_7", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus1_8", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } } };

    // Bus2 data pins (8 bits)
    Pin bus2[8] = { Pin{ { .name = "Bus2_1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus2_2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus2_3", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus2_4", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus2_5", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus2_6", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus2_7", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                    Pin{ { .name = "Bus2_8", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } } };

    // Wire Bus1 -> regA inputs, Bus2 -> regB inputs
    const char* dPins[8] = { "1D", "2D", "3D", "4D", "5D", "6D", "7D", "8D" };

    for (int i = 0; i < 8; ++i)
    {
        bus1[i].connect<Level>([&regA, i, dPins](const Level l) { regA.pin(dPins[i]).drive<Level>(l); });
        bus2[i].connect<Level>([&regB, i, dPins](const Level l) { regB.pin(dPins[i]).drive<Level>(l); });
    }

    // Helper: drive bus from an 8-bit value (bit 0 = pin 1)
    auto driveBus = [](Pin* bus, std::uint8_t val) {
        for (int i = 0; i < 8; ++i)
        {
            bus[i].drive<Level>((val >> i) & 1u ? Level::High : Level::Low);
        }
    };

    // Print all 8 Q outputs of a register as binary (MSB first), '-' for HighZ
    auto printReg = [](const char* label, LS374& reg) {
        const char* qPins[8] = { "8Q", "7Q", "6Q", "5Q", "4Q", "3Q", "2Q", "1Q" };
        std::cout << "  " << label << ": ";
        for (int i = 0; i < 8; ++i)
        {
            const Level l = reg.pin(qPins[i]).read<Level>();
            std::cout << (l == Level::High ? '1' : l == Level::Low ? '0' : '-');
        }
        std::cout << '\n';
    };

    std::cout << "Bidirectional Bus Driver\n";
    std::cout << "SN74LS374 datasheet application\n\n";

    pinBEC.drive<Level>(Level::Low);
    pinClock1.drive<Level>(Level::Low);
    pinClock2.drive<Level>(Level::Low);
    pinOC1.drive<Level>(Level::High);
    pinOC2.drive<Level>(Level::High);

    // BEC=Low: clock gated, no transfer
    driveBus(bus1, 0xA5);
    driveBus(bus2, 0x3C);
    std::cout << "Bus1=0xA5  Bus2=0x3C  BEC=Low\n";
    printReg("U1 Bus1->Bus2", regA);
    printReg("U2 Bus2->Bus1", regB);
    std::cout << '\n';

    // BEC=High, CLK1^, CLK2^: both buses transferred
    pinBEC.drive<Level>(Level::High);
    pinClock1.drive<Level>(Level::High);
    pinClock2.drive<Level>(Level::High);
    std::cout << "Bus1=0xA5  Bus2=0x3C  BEC=High  CLK1^  CLK2^\n";
    printReg("U1 Bus1->Bus2", regA);
    printReg("U2 Bus2->Bus1", regB);
    std::cout << '\n';

    // BEC=Low: CLK1 edge gated out, U1 holds
    pinBEC.drive<Level>(Level::Low);
    pinClock1.drive<Level>(Level::Low);
    pinClock1.drive<Level>(Level::High);
    driveBus(bus1, 0xFF);
    std::cout << "Bus1=0xFF  BEC=Low  CLK1^\n";
    printReg("U1 Bus1->Bus2", regA);
    std::cout << '\n';

    // OC1=Low: U1 outputs go HighZ
    pinOC1.drive<Level>(Level::Low);
    std::cout << "OC1=Low\n";
    printReg("U1 Bus1->Bus2", regA);
    std::cout << '\n';
}
