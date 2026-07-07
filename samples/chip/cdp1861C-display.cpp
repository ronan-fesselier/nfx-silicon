#include <App.h>
#include <GlRenderer.h>

#include <nfx/Silicon.h>

#include <array>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::chip::cdp1xxx;
using namespace nfx::silicon::samples;

static constexpr int k_pixW = 64;
static constexpr int k_pixH = 128;

// RCA logo, 5x7 glyphs, centered in the 64x128 display window (rows 60-66)
static constexpr std::array<uint8_t, k_pixH * 8> k_vram = [] {
    std::array<uint8_t, k_pixH * 8> v{};
    // clang-format off
    constexpr uint8_t logo[7][8] = {
        { 0x00, 0x00, 0x01, 0xE3, 0xCE, 0x00, 0x00, 0x00 },
        { 0x00, 0x00, 0x01, 0x14, 0x11, 0x00, 0x00, 0x00 },
        { 0x00, 0x00, 0x01, 0x14, 0x11, 0x00, 0x00, 0x00 },
        { 0x00, 0x00, 0x01, 0xE4, 0x1F, 0x00, 0x00, 0x00 },
        { 0x00, 0x00, 0x01, 0x44, 0x11, 0x00, 0x00, 0x00 },
        { 0x00, 0x00, 0x01, 0x24, 0x11, 0x00, 0x00, 0x00 },
        { 0x00, 0x00, 0x01, 0x13, 0xD1, 0x00, 0x00, 0x00 }
    };
    // clang-format on

    for (int r = 0; r < 7; ++r)
    {
        for (int b = 0; b < 8; ++b)
        {
            v[(60 + r) * 8 + b] = logo[r][b];
        }
    }
    return v;
}();

namespace
{
    // clang-format off
    CDP1861::Terminal k_diNames[] = {
        CDP1861::Terminal::DI0,
        CDP1861::Terminal::DI1,
        CDP1861::Terminal::DI2,
        CDP1861::Terminal::DI3,
        CDP1861::Terminal::DI4,
        CDP1861::Terminal::DI5,
        CDP1861::Terminal::DI6,
        CDP1861::Terminal::DI7
    };
    // clang-format on

    auto setSC(CDP1861& pixie, const bool dma) -> void
    {
        pixie.pin(CDP1861::Terminal::SC1).drive<Level>(dma ? Level::Low : Level::High);
        pixie.pin(CDP1861::Terminal::SC0).drive<Level>(dma ? Level::High : Level::Low);
    }

    auto advanceCycle(CDP1861& pixie) -> void
    {
        pixie.pin(CDP1861::Terminal::TPA).drive<Level>(Level::Low);
        pixie.pin(CDP1861::Terminal::TPA).drive<Level>(Level::High);
        pixie.pin(CDP1861::Terminal::TPB).drive<Level>(Level::Low);
        pixie.pin(CDP1861::Terminal::TPB).drive<Level>(Level::High);
        pixie.pin(CDP1861::Terminal::TPB).drive<Level>(Level::Low);
        pixie.pin(CDP1861::Terminal::TPA).drive<Level>(Level::Low);
    }

    auto bareTPA(CDP1861& pixie) -> void
    {
        // advance hCounter inside a DMA burst without a full machine cycle
        pixie.pin(CDP1861::Terminal::TPA).drive<Level>(Level::Low);
        pixie.pin(CDP1861::Terminal::TPA).drive<Level>(Level::High);
        pixie.pin(CDP1861::Terminal::TPA).drive<Level>(Level::Low);
    }

    auto setDI(CDP1861& pixie, const uint8_t byte) -> void
    {
        for (int i = 0; i < 8; ++i)
        {
            pixie.pin(k_diNames[i]).drive<Level>(((byte >> i) & 1) ? Level::High : Level::Low);
        }
    }

    auto serviceByte(
        CDP1861& pixie, std::array<bool, k_pixW * k_pixH>& fb, const int row, const int col, const uint8_t data) -> void
    {
        bareTPA(pixie);
        setDI(pixie, data);

        pixie.pin(CDP1861::Terminal::TPB).drive<Level>(Level::Low);
        pixie.pin(CDP1861::Terminal::TPB).drive<Level>(Level::High); // arm latch

        for (int bit = 0; bit < 8; ++bit)
        {
            pixie.pin(CDP1861::Terminal::nCLK).drive<Level>(Level::High);
            pixie.pin(CDP1861::Terminal::nCLK).drive<Level>(Level::Low); // shift: VIDEO = MSB
            fb[row * k_pixW + col + bit] = (pixie.pin(CDP1861::Terminal::VIDEO).read<Level>() == Level::High);
        }

        pixie.pin(CDP1861::Terminal::TPB).drive<Level>(Level::Low);
        pixie.pin(CDP1861::Terminal::TPA).drive<Level>(Level::Low);
    }

    auto simulateField(
        CDP1861& pixie, const std::array<uint8_t, k_pixH * 8>& vram, std::array<bool, k_pixW * k_pixH>& fb) -> void
    {
        pixie.pin(CDP1861::Terminal::nRESET).drive<Level>(Level::Low);
        pixie.pin(CDP1861::Terminal::nRESET).drive<Level>(Level::High);
        pixie.pin(CDP1861::Terminal::DISPON).drive<Level>(Level::Low);
        pixie.pin(CDP1861::Terminal::DISPON).drive<Level>(Level::High); // Low->High edge enables display

        setSC(pixie, false);

        int row = 0;
        int line = 0;
        while (line < 262 && row < 128)
        {
            for (int c = 0; c < 13; ++c)
            {
                advanceCycle(pixie);
            }
            bareTPA(pixie);
            bareTPA(pixie);
            ++line;

            if (pixie.pin(CDP1861::Terminal::nDMAO).read<Level>() == Level::Low)
            {
                setSC(pixie, true);
                for (int b = 0; b < 8; ++b)
                {
                    serviceByte(pixie, fb, row, b * 8, vram[row * 8 + b]);
                }
                setSC(pixie, false);
                ++row;
            }
        }
    }
} // namespace

int main()
{
    CDP1861 pixie(CDP1861::Descriptor{ "U1" });
    pixie.pin(CDP1861::Terminal::VDD).drive<float>(5.0f);
    pixie.pin(CDP1861::Terminal::nRESET).drive<Level>(Level::High);

    std::array<bool, k_pixW * k_pixH> fb{};

    App app(App::Config{
        .title = "nfx::silicon CDP1861C",
        .width = 640,
        .height = 1280,
        .glMajor = 3,
        .glMinor = 3,
    });

    return app.run(
        // onInit
        []() {},
        // onShutdown
        []() {},
        // onRender
        [&](const int width, const int height) {
            simulateField(pixie, k_vram, fb);
            GlRenderer{}.render(fb.data(), k_pixW, k_pixH, width, height);
        });
}
