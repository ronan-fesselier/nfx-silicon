#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::gate;
using namespace nfx::silicon::gate::sn74;

TEST_SUITE("gate::sn74::LS74")
{
    TEST_CASE("Construction creates 14 pins")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 14);
    }

    TEST_CASE("Pin names match DIP-14 datasheet")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };

        CHECK(u.pin("1CLR").descriptor().name == std::string_view("1CLR"));
        CHECK(u.pin("1D").descriptor().name == std::string_view("1D"));
        CHECK(u.pin("1CLK").descriptor().name == std::string_view("1CLK"));
        CHECK(u.pin("1PRE").descriptor().name == std::string_view("1PRE"));
        CHECK(u.pin("1Q").descriptor().name == std::string_view("1Q"));
        CHECK(u.pin("1NQ").descriptor().name == std::string_view("1NQ"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("2NQ").descriptor().name == std::string_view("2NQ"));
        CHECK(u.pin("2Q").descriptor().name == std::string_view("2Q"));
        CHECK(u.pin("2PRE").descriptor().name == std::string_view("2PRE"));
        CHECK(u.pin("2CLK").descriptor().name == std::string_view("2CLK"));
        CHECK(u.pin("2CLR").descriptor().name == std::string_view("2CLR"));
        CHECK(u.pin("2D").descriptor().name == std::string_view("2D"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(LS74::Terminal::CLR1) == &u.pin("1CLR"));
        CHECK(&u.pin(LS74::Terminal::D1) == &u.pin("1D"));
        CHECK(&u.pin(LS74::Terminal::CLK1) == &u.pin("1CLK"));
        CHECK(&u.pin(LS74::Terminal::PRE1) == &u.pin("1PRE"));
        CHECK(&u.pin(LS74::Terminal::Q1) == &u.pin("1Q"));
        CHECK(&u.pin(LS74::Terminal::NQ1) == &u.pin("1NQ"));
        CHECK(&u.pin(LS74::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS74::Terminal::NQ2) == &u.pin("2NQ"));
        CHECK(&u.pin(LS74::Terminal::Q2) == &u.pin("2Q"));
        CHECK(&u.pin(LS74::Terminal::PRE2) == &u.pin("2PRE"));
        CHECK(&u.pin(LS74::Terminal::CLK2) == &u.pin("2CLK"));
        CHECK(&u.pin(LS74::Terminal::CLR2) == &u.pin("2CLR"));
        CHECK(&u.pin(LS74::Terminal::D2) == &u.pin("2D"));
        CHECK(&u.pin(LS74::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);

        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("1NQ").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC driven at 5V: FF1 rising edge samples D=High -> 1Q=High 1NQ=Low")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);

        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);

        CHECK(u.pin("1Q").read<Level>() == Level::High);
        CHECK(u.pin("1NQ").read<Level>() == Level::Low);
    }

    TEST_CASE("FF1 rising edge samples D=Low -> 1Q=Low 1NQ=High")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::Low);

        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);

        CHECK(u.pin("1Q").read<Level>() == Level::Low);
        CHECK(u.pin("1NQ").read<Level>() == Level::High);
    }

    TEST_CASE("FF1 async CLR=Low -> 1Q=Low 1NQ=High regardless of CLK")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);

        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("1CLR").drive<Level>(Level::Low);

        CHECK(u.pin("1Q").read<Level>() == Level::Low);
        CHECK(u.pin("1NQ").read<Level>() == Level::High);
    }

    TEST_CASE("FF1 async PRE=Low -> 1Q=High 1NQ=Low regardless of CLK")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::Low);

        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::Low);

        u.pin("1PRE").drive<Level>(Level::Low);

        CHECK(u.pin("1Q").read<Level>() == Level::High);
        CHECK(u.pin("1NQ").read<Level>() == Level::Low);
    }

    TEST_CASE("FF1 forbidden state CLR=Low PRE=Low -> 1Q=HighZ 1NQ=HighZ")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1CLR").drive<Level>(Level::Low);
        u.pin("1PRE").drive<Level>(Level::Low);

        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("1NQ").read<Level>() == Level::HighZ);
    }

    TEST_CASE("FF2 rising edge samples D=High -> 2Q=High 2NQ=Low")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("2CLR").drive<Level>(Level::High);
        u.pin("2PRE").drive<Level>(Level::High);
        u.pin("2D").drive<Level>(Level::High);

        u.pin("2CLK").drive<Level>(Level::Low);
        u.pin("2CLK").drive<Level>(Level::High);

        CHECK(u.pin("2Q").read<Level>() == Level::High);
        CHECK(u.pin("2NQ").read<Level>() == Level::Low);
    }

    TEST_CASE("FF1 and FF2 are independent")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("2CLR").drive<Level>(Level::High);
        u.pin("2PRE").drive<Level>(Level::High);

        u.pin("1D").drive<Level>(Level::High);
        u.pin("2D").drive<Level>(Level::Low);

        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);
        u.pin("2CLK").drive<Level>(Level::Low);
        u.pin("2CLK").drive<Level>(Level::High);

        CHECK(u.pin("1Q").read<Level>() == Level::High);
        CHECK(u.pin("1NQ").read<Level>() == Level::Low);
        CHECK(u.pin("2Q").read<Level>() == Level::Low);
        CHECK(u.pin("2NQ").read<Level>() == Level::High);
    }

    TEST_CASE("VCC removed after operation -> outputs return to HighZ")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("VCC").release();

        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("1NQ").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("2NQ").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC restored after power loss -> FF starts from reset state")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);

        u.pin("VCC").drive<float>(5.0f);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("VCC").release();
        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);

        u.pin("VCC").drive<float>(5.0f);

        CHECK(u.pin("1Q").read<Level>() == Level::Low);
        CHECK(u.pin("1NQ").read<Level>() == Level::High);
    }

    TEST_CASE("VCC below range -> outputs are HighZ")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("VCC").drive<float>(3.0f);

        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("1NQ").read<Level>() == Level::HighZ);
    }

    TEST_CASE("CLK edges before VCC are ignored on power-on")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);

        u.pin("VCC").drive<float>(5.0f);

        CHECK(u.pin("1Q").read<Level>() == Level::Low);
        CHECK(u.pin("1NQ").read<Level>() == Level::High);
    }

    TEST_CASE("forbidden state exit: CLR released while PRE still Low -> Q=High NQ=Low")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1CLR").drive<Level>(Level::Low);
        u.pin("1PRE").drive<Level>(Level::Low);
        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("1NQ").read<Level>() == Level::HighZ);

        u.pin("1CLR").drive<Level>(Level::High);

        CHECK(u.pin("1Q").read<Level>() == Level::High);
        CHECK(u.pin("1NQ").read<Level>() == Level::Low);
    }

    TEST_CASE("reset() drives both Q=Low NQ=High")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1CLR").drive<Level>(Level::High);
        u.pin("1PRE").drive<Level>(Level::High);
        u.pin("2CLR").drive<Level>(Level::High);
        u.pin("2PRE").drive<Level>(Level::High);

        u.pin("1D").drive<Level>(Level::High);
        u.pin("2D").drive<Level>(Level::High);
        u.pin("1CLK").drive<Level>(Level::Low);
        u.pin("1CLK").drive<Level>(Level::High);
        u.pin("2CLK").drive<Level>(Level::Low);
        u.pin("2CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);
        CHECK(u.pin("2Q").read<Level>() == Level::High);

        u.reset();

        CHECK(u.pin("1Q").read<Level>() == Level::Low);
        CHECK(u.pin("1NQ").read<Level>() == Level::High);
        CHECK(u.pin("2Q").read<Level>() == Level::Low);
        CHECK(u.pin("2NQ").read<Level>() == Level::High);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS74 u{ LS74::Descriptor{ .name = "U1" } };

        CHECK(u.name() == std::string_view("U1"));
    }
}
