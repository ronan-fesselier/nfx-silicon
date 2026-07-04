#include <nfx/Silicon.h>

#include <iostream>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::gate;
using namespace nfx::silicon::gate::sn74;

// ---------------------------------------------------------------------------
// Ribbon Cable Bus Transceiver (SNx4LS245 datasheet typical application)
//
// Two LS245 act as permanently-enabled transceivers across an 8-bit ribbon cable.
//
//   Master bus <-> U1 (A-side) | (B-side) [ribbon] (A-side) U2 | (B-side) <-> Slave bus
//
// /OE tied Low on both packages (always enabled).
// DIR=High: Master -> Slave  (A->B on U1, A->B on U2)
// DIR=Low:  Slave  -> Master (B->A on U1, B->A on U2)
// ---------------------------------------------------------------------------

int main()
{
    LS245 u1{ { .name = "U1" } }; // Master-side transceiver
    LS245 u2{ { .name = "U2" } }; // Slave-side transceiver

    u1.pin("VCC").drive<float>(5.0f);
    u2.pin("VCC").drive<float>(5.0f);

    // /OE tied Low: permanently enabled
    u1.pin("/OE").drive<Level>(Level::Low);
    u2.pin("/OE").drive<Level>(Level::Low);

    // Shared direction control
    Pin pinDIR({ .name = "DIR", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output });
    pinDIR.connect<Level>([&](const Level l) {
        u1.pin("DIR").drive<Level>(l);
        u2.pin("DIR").drive<Level>(l);
    });

    // Master-side: 8 output drivers + 8 input observers
    Pin masterDrv[8] = { Pin{ { .name = "MD0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                         Pin{ { .name = "MD1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                         Pin{ { .name = "MD2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                         Pin{ { .name = "MD3", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                         Pin{ { .name = "MD4", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                         Pin{ { .name = "MD5", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                         Pin{ { .name = "MD6", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                         Pin{ { .name = "MD7", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } } };
    Pin masterObs[8] = { Pin{ { .name = "MO0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                         Pin{ { .name = "MO1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                         Pin{ { .name = "MO2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                         Pin{ { .name = "MO3", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                         Pin{ { .name = "MO4", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                         Pin{ { .name = "MO5", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                         Pin{ { .name = "MO6", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                         Pin{ { .name = "MO7", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } } };

    // Slave-side: 8 output drivers + 8 input observers
    Pin slaveDrv[8] = { Pin{ { .name = "SD0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                        Pin{ { .name = "SD1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                        Pin{ { .name = "SD2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                        Pin{ { .name = "SD3", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                        Pin{ { .name = "SD4", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                        Pin{ { .name = "SD5", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                        Pin{ { .name = "SD6", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
                        Pin{ { .name = "SD7", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } } };
    Pin slaveObs[8] = { Pin{ { .name = "SO0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                        Pin{ { .name = "SO1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                        Pin{ { .name = "SO2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                        Pin{ { .name = "SO3", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                        Pin{ { .name = "SO4", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                        Pin{ { .name = "SO5", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                        Pin{ { .name = "SO6", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } },
                        Pin{ { .name = "SO7", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } } };

    const char* aPins[8] = { "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8" };
    const char* bPins[8] = { "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8" };

    // Master bus wires (masterDrv, U1.A, masterObs)
    Wire wm0{ { .name = "WM0", .kind = Pin::Kind::Digital } };
    Wire wm1{ { .name = "WM1", .kind = Pin::Kind::Digital } };
    Wire wm2{ { .name = "WM2", .kind = Pin::Kind::Digital } };
    Wire wm3{ { .name = "WM3", .kind = Pin::Kind::Digital } };
    Wire wm4{ { .name = "WM4", .kind = Pin::Kind::Digital } };
    Wire wm5{ { .name = "WM5", .kind = Pin::Kind::Digital } };
    Wire wm6{ { .name = "WM6", .kind = Pin::Kind::Digital } };
    Wire wm7{ { .name = "WM7", .kind = Pin::Kind::Digital } };
    Wire* wMaster[8] = { &wm0, &wm1, &wm2, &wm3, &wm4, &wm5, &wm6, &wm7 };

    // Ribbon cable wires (U1.B, U2.A)
    Wire wr0{ { .name = "WR0", .kind = Pin::Kind::Digital } };
    Wire wr1{ { .name = "WR1", .kind = Pin::Kind::Digital } };
    Wire wr2{ { .name = "WR2", .kind = Pin::Kind::Digital } };
    Wire wr3{ { .name = "WR3", .kind = Pin::Kind::Digital } };
    Wire wr4{ { .name = "WR4", .kind = Pin::Kind::Digital } };
    Wire wr5{ { .name = "WR5", .kind = Pin::Kind::Digital } };
    Wire wr6{ { .name = "WR6", .kind = Pin::Kind::Digital } };
    Wire wr7{ { .name = "WR7", .kind = Pin::Kind::Digital } };
    Wire* wRibbon[8] = { &wr0, &wr1, &wr2, &wr3, &wr4, &wr5, &wr6, &wr7 };

    // Slave bus wires (U2.B, slaveDrv, slaveObs)
    Wire ws0{ { .name = "WS0", .kind = Pin::Kind::Digital } };
    Wire ws1{ { .name = "WS1", .kind = Pin::Kind::Digital } };
    Wire ws2{ { .name = "WS2", .kind = Pin::Kind::Digital } };
    Wire ws3{ { .name = "WS3", .kind = Pin::Kind::Digital } };
    Wire ws4{ { .name = "WS4", .kind = Pin::Kind::Digital } };
    Wire ws5{ { .name = "WS5", .kind = Pin::Kind::Digital } };
    Wire ws6{ { .name = "WS6", .kind = Pin::Kind::Digital } };
    Wire ws7{ { .name = "WS7", .kind = Pin::Kind::Digital } };
    Wire* wSlave[8] = { &ws0, &ws1, &ws2, &ws3, &ws4, &ws5, &ws6, &ws7 };

    for (int i = 0; i < 8; ++i)
    {
        wMaster[i]->attach(masterDrv[i]);
        wMaster[i]->attach(u1.pin(aPins[i]));
        wMaster[i]->attach(masterObs[i]);

        wRibbon[i]->attach(u1.pin(bPins[i]));
        wRibbon[i]->attach(u2.pin(aPins[i]));

        wSlave[i]->attach(u2.pin(bPins[i]));
        wSlave[i]->attach(slaveDrv[i]);
        wSlave[i]->attach(slaveObs[i]);
    }

    auto driveMaster = [&](const std::uint8_t val) {
        for (int i = 0; i < 8; ++i)
        {
            masterDrv[i].drive<Level>((val >> i) & 1u ? Level::High : Level::Low);
        }
    };
    auto releaseMaster = [&]() {
        for (int i = 0; i < 8; ++i)
        {
            masterDrv[i].release();
        }
    };
    auto driveSlave = [&](const std::uint8_t val) {
        for (int i = 0; i < 8; ++i)
        {
            slaveDrv[i].drive<Level>((val >> i) & 1u ? Level::High : Level::Low);
        }
    };
    auto releaseSlave = [&]() {
        for (int i = 0; i < 8; ++i)
        {
            slaveDrv[i].release();
        }
    };

    auto printBus = [](const char* label, Pin* obs) {
        std::cout << label << ' ';
        for (int i = 7; i >= 0; --i)
        {
            const Level l = obs[i].read<Level>();
            std::cout << (l == Level::High ? '1' : l == Level::Low ? '0' : '-');
        }
        std::cout << '\n';
    };

    std::cout << "Ribbon Cable Bus Transceiver\n";
    std::cout << "SNx4LS245 datasheet application\n\n";

    // Master -> Slave
    pinDIR.drive<Level>(Level::High);
    driveMaster(0xA5);
    std::cout << "DIR=High  Master=0xA5  (Master -> Slave)\n";
    printBus("  Master:", masterObs);
    printBus("  Slave: ", slaveObs);
    std::cout << '\n';

    // Slave -> Master: release master-side driver first (bus contention otherwise)
    releaseMaster();
    pinDIR.drive<Level>(Level::Low);
    driveSlave(0x3C);
    std::cout << "DIR=Low   Slave=0x3C   (Slave -> Master)\n";
    printBus("  Master:", masterObs);
    printBus("  Slave: ", slaveObs);
    std::cout << '\n';

    // /OE=High: isolation
    releaseSlave();
    u1.pin("/OE").drive<Level>(Level::High);
    u2.pin("/OE").drive<Level>(Level::High);
    std::cout << "/OE=High  isolation\n";
    printBus("  Master:", masterObs);
    printBus("  Slave: ", slaveObs);
    std::cout << '\n';
}
