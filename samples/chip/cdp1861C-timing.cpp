#include <nfx/Silicon.h>

#include <cstdio>

// CDP1861C timing sample - first display scanline signal trace.
// 262 lines x 14 cycles/line, 8 DMA bytes per scanline, checkerboard pattern (0xAA).
// Signals sampled after each CLK falling edge (ref. datasheet Fig. 2 / Fig. 4).

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::chip::cdp1xxx;

namespace
{
    auto setSC(CDP1861& pixie, const bool dma) -> void
    {
        pixie.pin("SC1").drive<Level>(dma ? Level::Low : Level::High);
        pixie.pin("SC0").drive<Level>(dma ? Level::High : Level::Low);
    }

    auto advanceCycle(CDP1861& pixie) -> void
    {
        pixie.pin("TPA").drive<Level>(Level::Low);
        pixie.pin("TPA").drive<Level>(Level::High);
        pixie.pin("TPB").drive<Level>(Level::Low);
        pixie.pin("TPB").drive<Level>(Level::High);
        pixie.pin("TPB").drive<Level>(Level::Low);
        pixie.pin("TPA").drive<Level>(Level::Low);
    }

    auto bareTPA(CDP1861& pixie) -> void
    {
        pixie.pin("TPA").drive<Level>(Level::Low);
        pixie.pin("TPA").drive<Level>(Level::High);
        pixie.pin("TPA").drive<Level>(Level::Low);
    }

    auto setDI(CDP1861& pixie, const uint8_t byte) -> void
    {
        const char* names[] = { "DI0", "DI1", "DI2", "DI3", "DI4", "DI5", "DI6", "DI7" };
        for (int i = 0; i < 8; ++i)
            pixie.pin(names[i]).drive<Level>(((byte >> i) & 1) ? Level::High : Level::Low);
    }

    auto lvl(CDP1861& pixie, const char* name) -> char
    {
        return pixie.pin(name).read<Level>() == Level::High ? 'H' : 'L';
    }
} // namespace

int main()
{
    CDP1861 pixie(CDP1861::Descriptor{ "U1" });
    pixie.pin("VDD").drive<float>(5.0f);
    pixie.pin("nRESET").drive<Level>(Level::High);
    pixie.pin("DISPON").drive<Level>(Level::Low);
    pixie.pin("DISPON").drive<Level>(Level::High);

    // SC1=H SC0=L : normal (non-DMA) machine cycle
    setSC(pixie, false);

    // 4 lines to reach vCounter=4 (display window start)
    for (int line = 0; line < 4; ++line)
    {
        for (int c = 0; c < 13; ++c)
        {
            advanceCycle(pixie);
        }
        bareTPA(pixie); // vCounter++
        bareTPA(pixie); // assert /DMAO when in display window
    }

    // At this point vCounter=4, /DMAO should be asserted.
    // Load checkerboard (0xAA = 1010_1010, MSB first -> H L H L H L H L)
    constexpr uint8_t kPattern = 0xAA;

    std::printf("CDP1861C timing sample\n");
    std::printf("First display scanline - checkerboard DI=0x%02X (MSB first: H L H L H L H L)\n\n", kPattern);
    std::printf("byte  clk  nINT nDMAO nEFX  VIDEO nCOMPSYNC\n");
    std::printf("----  ---  ---- ----- ----  ----- ---------\n");

    setSC(pixie, true);

    for (int b = 0; b < 8; ++b)
    {
        setDI(pixie, kPattern);

        // TPB rising: arm the latch
        pixie.pin("TPB").drive<Level>(Level::Low);
        pixie.pin("TPB").drive<Level>(Level::High); // arm latch

        for (int bit = 0; bit < 8; ++bit)
        {
            // CLK falling: latch DI (bit 0 only) then shift -> VIDEO = current MSB
            pixie.pin("nCLK").drive<Level>(Level::High);
            pixie.pin("nCLK").drive<Level>(Level::Low); // latch DI then shift; VIDEO = MSB

            std::printf(
                "  %2d    %d    %c      %c    %c      %c         %c\n",
                b,
                bit,
                lvl(pixie, "nINT"),
                lvl(pixie, "nDMAO"),
                lvl(pixie, "nEFX"),
                lvl(pixie, "VIDEO"),
                lvl(pixie, "nCOMPSYNC"));
        }

        // TPB falling: end of DMA byte slot
        pixie.pin("TPB").drive<Level>(Level::Low);
    }

    setSC(pixie, false);

    return 0;
}
