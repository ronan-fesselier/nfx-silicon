#include <nfx/Silicon.h>

#include <cstdint>
#include <iomanip>
#include <iostream>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::chip::cdp1xxx;

namespace
{
    // clang-format off
    constexpr CDP1852::Terminal k_di[] = {
        CDP1852::Terminal::DI0,
        CDP1852::Terminal::DI1,
        CDP1852::Terminal::DI2,
        CDP1852::Terminal::DI3,
        CDP1852::Terminal::DI4,
        CDP1852::Terminal::DI5,
        CDP1852::Terminal::DI6,
        CDP1852::Terminal::DI7
    };
    constexpr CDP1852::Terminal k_do[] = {
        CDP1852::Terminal::DO0,
        CDP1852::Terminal::DO1,
        CDP1852::Terminal::DO2,
        CDP1852::Terminal::DO3,
        CDP1852::Terminal::DO4,
        CDP1852::Terminal::DO5,
        CDP1852::Terminal::DO6,
        CDP1852::Terminal::DO7
    };
    // clang-format on

    auto readDO(CDP1852& port) -> uint8_t
    {
        uint8_t value = 0;
        for (int i = 0; i < 8; ++i)
        {
            if (port.pin(k_do[i]).read<Level>() == Level::High)
            {
                value |= static_cast<uint8_t>(1 << i);
            }
        }
        return value;
    }

    auto writeDI(CDP1852& port, const uint8_t data) -> void
    {
        for (int i = 0; i < 8; ++i)
        {
            port.pin(k_di[i]).drive<Level>(((data >> i) & 1) ? Level::High : Level::Low);
        }
    }

    auto printState(const char* label, CDP1852& port) -> void
    {
        const uint8_t doVal = readDO(port);
        const Level sr = port.pin(CDP1852::Terminal::nSR_SR).read<Level>();
        std::cout << label
                  << "  DO=0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(doVal)
                  << std::dec
                  << "  nSR_SR=" << (sr == Level::High ? "H" : sr == Level::Low ? "L" : "Z")
                  << '\n';
    }

    auto clock(CDP1852& port) -> void
    {
        port.pin(CDP1852::Terminal::CLOCK).drive<Level>(Level::High);
        port.pin(CDP1852::Terminal::CLOCK).drive<Level>(Level::Low); // trailing edge latches
    }

    auto select(CDP1852& port) -> void
    {
        // input mode: nCS1_CS1=High + CS2=High; output mode: nCS1_CS1=Low + CS2=High
        port.pin(CDP1852::Terminal::CS2).drive<Level>(Level::High);
    }

    auto deselect(CDP1852& port) -> void
    {
        port.pin(CDP1852::Terminal::CS2).drive<Level>(Level::Low);
    }
} // namespace

int main()
{
    CDP1852 port(CDP1852::Descriptor{ "U3" });
    port.pin(CDP1852::Terminal::VDD).drive<float>(5.0f);
    port.pin(CDP1852::Terminal::CLOCK).drive<Level>(Level::Low);
    port.pin(CDP1852::Terminal::CLEAR).drive<Level>(Level::Low);
    deselect(port);

    // input mode (MODE=Low): peripheral -> CPU
    std::cout << "=== input mode (MODE=Low) ===\n";
    port.pin(CDP1852::Terminal::MODE).drive<Level>(Level::Low);
    port.pin(CDP1852::Terminal::nCS1_CS1).drive<Level>(Level::High); // selected in input mode

    // peripheral drives DI and strobes CLOCK
    writeDI(port, 0xA5);
    clock(port); // trailing edge latches DI, asserts /SR (Low)
    printState("after peripheral strobe  ", port);

    // CPU reads DO by asserting CS: /SR returns High (acknowledged)
    select(port);
    printState("after CPU read (CS=High) ", port);
    deselect(port);

    // second byte
    writeDI(port, 0x3C);
    clock(port);
    printState("after second strobe      ", port);
    select(port);
    printState("after CPU read           ", port);
    deselect(port);

    // async CLEAR
    port.pin(CDP1852::Terminal::CLEAR).drive<Level>(Level::High);
    printState("after CLEAR              ", port);
    port.pin(CDP1852::Terminal::CLEAR).drive<Level>(Level::Low);

    // output mode (MODE=High): CPU -> peripheral
    std::cout << "\n=== output mode (MODE=High) ===\n";
    port.pin(CDP1852::Terminal::MODE).drive<Level>(Level::High);
    port.pin(CDP1852::Terminal::nCS1_CS1).drive<Level>(Level::Low); // selected in output mode

    // CPU writes DI and strobes CLOCK with CS active
    writeDI(port, 0xA5);
    select(port);
    clock(port); // trailing edge latches DI into register, drives DO, SR=Low
    printState("after CPU write 0xA5     ", port);
    deselect(port);

    writeDI(port, 0x3C);
    select(port);
    clock(port);
    printState("after CPU write 0x3C     ", port);
    deselect(port);
    printState("after deselect           ", port); // DO still driven (output mode)

    // async CLEAR
    port.pin(CDP1852::Terminal::CLEAR).drive<Level>(Level::High);
    printState("after CLEAR              ", port);
    port.pin(CDP1852::Terminal::CLEAR).drive<Level>(Level::Low);

    return 0;
}
