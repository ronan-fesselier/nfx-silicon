#include <nfx/Silicon.h>

#include <array>
#include <iostream>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::chip::cdp1xxx;

namespace
{
    const char* k_diNames[] = { "DI0", "DI1", "DI2", "DI3", "DI4", "DI5", "DI6", "DI7" };

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

    auto serviceByte(
        CDP1861& pixie, std::array<std::array<bool, 64>, 128>& fb, const int row, const int col, const uint8_t data)
        -> void
    {
        // bare TPA: advance hCounter inside the DMA burst without a full machine cycle
        pixie.pin("TPA").drive<Level>(Level::Low);
        pixie.pin("TPA").drive<Level>(Level::High);

        for (int i = 0; i < 8; ++i)
        {
            pixie.pin(k_diNames[i]).drive<Level>(((data >> i) & 1) ? Level::High : Level::Low);
        }

        pixie.pin("TPB").drive<Level>(Level::Low);
        pixie.pin("TPB").drive<Level>(Level::High); // arm latch

        for (int bit = 0; bit < 8; ++bit)
        {
            pixie.pin("nCLK").drive<Level>(Level::High);
            pixie.pin("nCLK").drive<Level>(Level::Low); // latch DI (bit 0) then shift: VIDEO = MSB
            fb[row][col + bit] = (pixie.pin("VIDEO").read<Level>() == Level::High);
        }

        pixie.pin("TPB").drive<Level>(Level::Low);
        pixie.pin("TPA").drive<Level>(Level::Low);
    }
} // namespace

int main()
{
    // checkerboard: even rows = 0xAA (H L H L...), odd rows = 0x55 (L H L H...)
    std::array<uint8_t, 128 * 8> vram{};
    for (int r = 0; r < 128; ++r)
    {
        for (int b = 0; b < 8; ++b)
        {
            vram[r * 8 + b] = (r % 2 == 0) ? 0xAA : 0x55;
        }
    }

    CDP1861 pixie(CDP1861::Descriptor{ "U1" });
    pixie.pin("VDD").drive<float>(5.0f);
    pixie.pin("nRESET").drive<Level>(Level::High);
    pixie.pin("DISPON").drive<Level>(Level::Low);
    pixie.pin("DISPON").drive<Level>(Level::High); // Low->High edge enables display

    std::array<std::array<bool, 64>, 128> framebuffer{};
    for (auto& row : framebuffer)
    {
        row.fill(false);
    }

    setSC(pixie, false);

    int row = 0;
    while (row < 128)
    {
        if (pixie.pin("nDMAO").read<Level>() == Level::Low)
        {
            setSC(pixie, true);
            for (int b = 0; b < 8; ++b)
            {
                serviceByte(pixie, framebuffer, row, b * 8, vram[row * 8 + b]);
            }
            setSC(pixie, false);
            ++row;
        }
        else
        {
            advanceCycle(pixie);
        }
    }

    for (int r = 0; r < 128; ++r)
    {
        for (int c = 0; c < 64; ++c)
        {
            std::cout << (framebuffer[r][c] ? '#' : ' ');
        }
        std::cout << '\n';
    }

    return 0;
}
