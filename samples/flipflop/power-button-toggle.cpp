#include <nfx/Silicon.h>

#include <iostream>

using namespace nfx::silicon;
using namespace nfx::silicon::buffer::sn74xxx;
using namespace nfx::silicon::flipflop::cd4xxx;
using namespace nfx::silicon::signal;

// ---------------------------------------------------------------------------
// Power Button Circuit (CD4013B datasheet Figure 9)
//
// A SN74LVC1G17 Schmitt-trigger buffer conditions the pushbutton signal before
// clocking a CD4013B wired as a toggle: /Q fed back to D. Each rising edge on
// CLOCK flips Q, alternately enabling and disabling the MCU power rail.
//
// The RC debounce network and pushbutton are out of scope (analog solver
// required). The signal at pin A of the LVC1G17 is driven as a clean digital
// pulse, as seen after the RC stage in the real circuit.
//
// Topology:
//   Pin (clean pulse) -> LVC1G17 A -> LVC1G17 Y -> CD4013B CLOCK1
//   CD4013B: SET1=Low  RESET1=Low  D1=/Q1 (toggle feedback)
//   CD4013B: Q1 -> MCU power enable
// ---------------------------------------------------------------------------

int main()
{
    LVC1G17 u1{ { .name = "U1" } };
    CD4013B u2{ { .name = "U2" } };

    u1.pin(LVC1G17::Terminal::VCC).drive<float>(3.0f);
    u2.pin(CD4013B::Terminal::VDD).drive<float>(3.0f);

    u2.pin(CD4013B::Terminal::SET1).drive<Level>(Level::Low);
    u2.pin(CD4013B::Terminal::RESET1).drive<Level>(Level::Low);

    u2.pin(CD4013B::Terminal::nQ1).connect<Level>([&](const Level l) {
        u2.pin(CD4013B::Terminal::D1).drive<Level>(l);
    });

    Pin btnSignal{ { .name = "BTN", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
    btnSignal.connect<Level>([&](const Level l) { u1.pin(LVC1G17::Terminal::A).drive<Level>(l); });

    u1.pin("Y").connect<Level>([&](const Level l) { u2.pin(CD4013B::Terminal::CLOCK1).drive<Level>(l); });

    auto press = [&]() {
        btnSignal.drive<Level>(Level::Low);
        btnSignal.drive<Level>(Level::High);
    };

    const auto lvl = [](const Level l) { return l == Level::High ? '1' : l == Level::Low ? '0' : '-'; };

    const auto print = [&](const char* event) {
        std::cout << "  " << event << "     " << lvl(u2.pin(CD4013B::Terminal::Q1).read<Level>()) << "    "
                  << lvl(u2.pin(CD4013B::Terminal::nQ1).read<Level>()) << "   "
                  << lvl(u2.pin(CD4013B::Terminal::D1).read<Level>()) << '\n';
    };

    std::cout << "Power Button Toggle\n";
    std::cout << "CD4013B datasheet application (Figure 9)\n\n";
    std::cout << "  Event                       Q1  /Q1  D1\n";
    std::cout << "  --------------------------+---+----+---\n";

    print("Power on (MCU off)      ");
    press();
    print("Button press 1 (MCU on) ");
    press();
    print("Button press 2 (MCU off)");
    press();
    print("Button press 3 (MCU on) ");
    press();
    print("Button press 4 (MCU off)");

    return 0;
}
