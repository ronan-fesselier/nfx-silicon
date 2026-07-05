#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::decoder::sn74xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("decoder::sn74xxx::LS138")
{
    TEST_CASE("Construction creates 16 pins")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 16);
    }

    TEST_CASE("Pin names match DIP-16 datasheet")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };

        CHECK(u.pin("A").descriptor().name == std::string_view("A"));
        CHECK(u.pin("B").descriptor().name == std::string_view("B"));
        CHECK(u.pin("C").descriptor().name == std::string_view("C"));
        CHECK(u.pin("/G2A").descriptor().name == std::string_view("/G2A"));
        CHECK(u.pin("/G2B").descriptor().name == std::string_view("/G2B"));
        CHECK(u.pin("G1").descriptor().name == std::string_view("G1"));
        CHECK(u.pin("Y7").descriptor().name == std::string_view("Y7"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("Y6").descriptor().name == std::string_view("Y6"));
        CHECK(u.pin("Y5").descriptor().name == std::string_view("Y5"));
        CHECK(u.pin("Y4").descriptor().name == std::string_view("Y4"));
        CHECK(u.pin("Y3").descriptor().name == std::string_view("Y3"));
        CHECK(u.pin("Y2").descriptor().name == std::string_view("Y2"));
        CHECK(u.pin("Y1").descriptor().name == std::string_view("Y1"));
        CHECK(u.pin("Y0").descriptor().name == std::string_view("Y0"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(LS138::Terminal::A) == &u.pin("A"));
        CHECK(&u.pin(LS138::Terminal::B) == &u.pin("B"));
        CHECK(&u.pin(LS138::Terminal::C) == &u.pin("C"));
        CHECK(&u.pin(LS138::Terminal::nG2A) == &u.pin("/G2A"));
        CHECK(&u.pin(LS138::Terminal::nG2B) == &u.pin("/G2B"));
        CHECK(&u.pin(LS138::Terminal::G1) == &u.pin("G1"));
        CHECK(&u.pin(LS138::Terminal::Y7) == &u.pin("Y7"));
        CHECK(&u.pin(LS138::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS138::Terminal::Y6) == &u.pin("Y6"));
        CHECK(&u.pin(LS138::Terminal::Y5) == &u.pin("Y5"));
        CHECK(&u.pin(LS138::Terminal::Y4) == &u.pin("Y4"));
        CHECK(&u.pin(LS138::Terminal::Y3) == &u.pin("Y3"));
        CHECK(&u.pin(LS138::Terminal::Y2) == &u.pin("Y2"));
        CHECK(&u.pin(LS138::Terminal::Y1) == &u.pin("Y1"));
        CHECK(&u.pin(LS138::Terminal::Y0) == &u.pin("Y0"));
        CHECK(&u.pin(LS138::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };
        u.pin("G1").drive<Level>(Level::High);
        u.pin("/G2A").drive<Level>(Level::Low);
        u.pin("/G2B").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);

        CHECK(u.pin("Y0").read<Level>() == Level::HighZ);
        CHECK(u.pin("Y7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Disabled (G1=Low): all outputs High")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("G1").drive<Level>(Level::Low);
        u.pin("/G2A").drive<Level>(Level::Low);
        u.pin("/G2B").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);

        CHECK(u.pin("Y0").read<Level>() == Level::High);
        CHECK(u.pin("Y1").read<Level>() == Level::High);
        CHECK(u.pin("Y2").read<Level>() == Level::High);
        CHECK(u.pin("Y3").read<Level>() == Level::High);
        CHECK(u.pin("Y4").read<Level>() == Level::High);
        CHECK(u.pin("Y5").read<Level>() == Level::High);
        CHECK(u.pin("Y6").read<Level>() == Level::High);
        CHECK(u.pin("Y7").read<Level>() == Level::High);
    }

    TEST_CASE("Disabled (/G2A=High): all outputs High")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("G1").drive<Level>(Level::High);
        u.pin("/G2A").drive<Level>(Level::High);
        u.pin("/G2B").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);

        CHECK(u.pin("Y0").read<Level>() == Level::High);
        CHECK(u.pin("Y7").read<Level>() == Level::High);
    }

    TEST_CASE("Disabled (/G2B=High): all outputs High")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("G1").drive<Level>(Level::High);
        u.pin("/G2A").drive<Level>(Level::Low);
        u.pin("/G2B").drive<Level>(Level::High);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);

        CHECK(u.pin("Y0").read<Level>() == Level::High);
        CHECK(u.pin("Y7").read<Level>() == Level::High);
    }

    TEST_CASE("Enabled: all 8 addresses decode correctly")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("G1").drive<Level>(Level::High);
        u.pin("/G2A").drive<Level>(Level::Low);
        u.pin("/G2B").drive<Level>(Level::Low);

        const char* yPins[8] = { "Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7" };

        for (std::uint8_t addr = 0; addr < 8; ++addr)
        {
            u.pin("A").drive<Level>(addr & 1u ? Level::High : Level::Low);
            u.pin("B").drive<Level>(addr & 2u ? Level::High : Level::Low);
            u.pin("C").drive<Level>(addr & 4u ? Level::High : Level::Low);

            for (std::uint8_t i = 0; i < 8; ++i)
            {
                CHECK(u.pin(yPins[i]).read<Level>() == (i == addr ? Level::Low : Level::High));
            }
        }
    }

    TEST_CASE("Any address input HighZ: all outputs HighZ")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("G1").drive<Level>(Level::High);
        u.pin("/G2A").drive<Level>(Level::Low);
        u.pin("/G2B").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        CHECK(u.pin("Y0").read<Level>() == Level::Low);

        u.pin("A").release();

        CHECK(u.pin("Y0").read<Level>() == Level::HighZ);
        CHECK(u.pin("Y7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC removed after operation -> outputs return to HighZ")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("G1").drive<Level>(Level::High);
        u.pin("/G2A").drive<Level>(Level::Low);
        u.pin("/G2B").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        CHECK(u.pin("Y0").read<Level>() == Level::Low);

        u.pin("VCC").release();

        CHECK(u.pin("Y0").read<Level>() == Level::HighZ);
        CHECK(u.pin("Y7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC below range -> outputs are HighZ")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("G1").drive<Level>(Level::High);
        u.pin("/G2A").drive<Level>(Level::Low);
        u.pin("/G2B").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        CHECK(u.pin("Y0").read<Level>() == Level::Low);

        u.pin("VCC").drive<float>(3.0f);

        CHECK(u.pin("Y0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS138 u{ LS138::Descriptor{ .name = "U1" } };

        CHECK(u.name() == std::string_view("U1"));
    }
}

TEST_SUITE("decoder::sn74xxx::LS139")
{
    TEST_CASE("Construction creates 16 pins")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 16);
    }

    TEST_CASE("Pin names match DIP-16 datasheet")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };

        CHECK(u.pin("/1G").descriptor().name == std::string_view("/1G"));
        CHECK(u.pin("1A").descriptor().name == std::string_view("1A"));
        CHECK(u.pin("1B").descriptor().name == std::string_view("1B"));
        CHECK(u.pin("1Y0").descriptor().name == std::string_view("1Y0"));
        CHECK(u.pin("1Y1").descriptor().name == std::string_view("1Y1"));
        CHECK(u.pin("1Y2").descriptor().name == std::string_view("1Y2"));
        CHECK(u.pin("1Y3").descriptor().name == std::string_view("1Y3"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("2Y3").descriptor().name == std::string_view("2Y3"));
        CHECK(u.pin("2Y2").descriptor().name == std::string_view("2Y2"));
        CHECK(u.pin("2Y1").descriptor().name == std::string_view("2Y1"));
        CHECK(u.pin("2Y0").descriptor().name == std::string_view("2Y0"));
        CHECK(u.pin("2B").descriptor().name == std::string_view("2B"));
        CHECK(u.pin("2A").descriptor().name == std::string_view("2A"));
        CHECK(u.pin("/2G").descriptor().name == std::string_view("/2G"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(LS139::Terminal::nG1) == &u.pin("/1G"));
        CHECK(&u.pin(LS139::Terminal::A1) == &u.pin("1A"));
        CHECK(&u.pin(LS139::Terminal::B1) == &u.pin("1B"));
        CHECK(&u.pin(LS139::Terminal::Y1_0) == &u.pin("1Y0"));
        CHECK(&u.pin(LS139::Terminal::Y1_1) == &u.pin("1Y1"));
        CHECK(&u.pin(LS139::Terminal::Y1_2) == &u.pin("1Y2"));
        CHECK(&u.pin(LS139::Terminal::Y1_3) == &u.pin("1Y3"));
        CHECK(&u.pin(LS139::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS139::Terminal::Y2_3) == &u.pin("2Y3"));
        CHECK(&u.pin(LS139::Terminal::Y2_2) == &u.pin("2Y2"));
        CHECK(&u.pin(LS139::Terminal::Y2_1) == &u.pin("2Y1"));
        CHECK(&u.pin(LS139::Terminal::Y2_0) == &u.pin("2Y0"));
        CHECK(&u.pin(LS139::Terminal::B2) == &u.pin("2B"));
        CHECK(&u.pin(LS139::Terminal::A2) == &u.pin("2A"));
        CHECK(&u.pin(LS139::Terminal::nG2) == &u.pin("/2G"));
        CHECK(&u.pin(LS139::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };
        u.pin("/1G").drive<Level>(Level::Low);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y0").read<Level>() == Level::HighZ);
        CHECK(u.pin("1Y3").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Decoder 1 disabled (/1G=High): all outputs High")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/1G").drive<Level>(Level::High);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y0").read<Level>() == Level::High);
        CHECK(u.pin("1Y1").read<Level>() == Level::High);
        CHECK(u.pin("1Y2").read<Level>() == Level::High);
        CHECK(u.pin("1Y3").read<Level>() == Level::High);
    }

    TEST_CASE("Decoder 1 enabled: all 4 addresses decode correctly")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/1G").drive<Level>(Level::Low);

        const char* yPins[4] = { "1Y0", "1Y1", "1Y2", "1Y3" };

        for (std::uint8_t addr = 0; addr < 4; ++addr)
        {
            u.pin("1A").drive<Level>(addr & 1u ? Level::High : Level::Low);
            u.pin("1B").drive<Level>(addr & 2u ? Level::High : Level::Low);

            for (std::uint8_t i = 0; i < 4; ++i)
            {
                CHECK(u.pin(yPins[i]).read<Level>() == (i == addr ? Level::Low : Level::High));
            }
        }
    }

    TEST_CASE("Decoder 2 enabled: all 4 addresses decode correctly")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/2G").drive<Level>(Level::Low);

        const char* yPins[4] = { "2Y0", "2Y1", "2Y2", "2Y3" };

        for (std::uint8_t addr = 0; addr < 4; ++addr)
        {
            u.pin("2A").drive<Level>(addr & 1u ? Level::High : Level::Low);
            u.pin("2B").drive<Level>(addr & 2u ? Level::High : Level::Low);

            for (std::uint8_t i = 0; i < 4; ++i)
            {
                CHECK(u.pin(yPins[i]).read<Level>() == (i == addr ? Level::Low : Level::High));
            }
        }
    }

    TEST_CASE("Two decoders are independent")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/1G").drive<Level>(Level::Low);
        u.pin("/2G").drive<Level>(Level::Low);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);
        u.pin("2A").drive<Level>(Level::High);
        u.pin("2B").drive<Level>(Level::High);

        CHECK(u.pin("1Y0").read<Level>() == Level::Low);
        CHECK(u.pin("1Y1").read<Level>() == Level::High);
        CHECK(u.pin("2Y3").read<Level>() == Level::Low);
        CHECK(u.pin("2Y0").read<Level>() == Level::High);
    }

    TEST_CASE("Any input HighZ: outputs HighZ")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/1G").drive<Level>(Level::Low);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);
        CHECK(u.pin("1Y0").read<Level>() == Level::Low);

        u.pin("1A").release();

        CHECK(u.pin("1Y0").read<Level>() == Level::HighZ);
        CHECK(u.pin("1Y3").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC removed after operation: outputs return to HighZ")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/1G").drive<Level>(Level::Low);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);
        CHECK(u.pin("1Y0").read<Level>() == Level::Low);

        u.pin("VCC").release();

        CHECK(u.pin("1Y0").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Y0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC below range: outputs are HighZ")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/1G").drive<Level>(Level::Low);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);
        CHECK(u.pin("1Y0").read<Level>() == Level::Low);

        u.pin("VCC").drive<float>(3.0f);

        CHECK(u.pin("1Y0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS139 u{ LS139::Descriptor{ .name = "U1" } };

        CHECK(u.name() == std::string_view("U1"));
    }
}
