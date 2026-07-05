#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::flipflop::sn74xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("flipflop::sn74xxx::LS74")
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
        CHECK(&u.pin(LS74::Terminal::nQ1) == &u.pin("1NQ"));
        CHECK(&u.pin(LS74::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS74::Terminal::nQ2) == &u.pin("2NQ"));
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

TEST_SUITE("flipflop::sn74xxx::LS374")
{
    TEST_CASE("Construction creates 20 pins")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        CHECK(u.pins().size() == 20);
    }

    TEST_CASE("Pin names match DIP-20 datasheet")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        CHECK(u.pin("OC").descriptor().name == std::string_view("OC"));
        CHECK(u.pin("1Q").descriptor().name == std::string_view("1Q"));
        CHECK(u.pin("1D").descriptor().name == std::string_view("1D"));
        CHECK(u.pin("2D").descriptor().name == std::string_view("2D"));
        CHECK(u.pin("2Q").descriptor().name == std::string_view("2Q"));
        CHECK(u.pin("3Q").descriptor().name == std::string_view("3Q"));
        CHECK(u.pin("3D").descriptor().name == std::string_view("3D"));
        CHECK(u.pin("4D").descriptor().name == std::string_view("4D"));
        CHECK(u.pin("4Q").descriptor().name == std::string_view("4Q"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("CLK").descriptor().name == std::string_view("CLK"));
        CHECK(u.pin("5Q").descriptor().name == std::string_view("5Q"));
        CHECK(u.pin("5D").descriptor().name == std::string_view("5D"));
        CHECK(u.pin("6D").descriptor().name == std::string_view("6D"));
        CHECK(u.pin("6Q").descriptor().name == std::string_view("6Q"));
        CHECK(u.pin("7Q").descriptor().name == std::string_view("7Q"));
        CHECK(u.pin("7D").descriptor().name == std::string_view("7D"));
        CHECK(u.pin("8D").descriptor().name == std::string_view("8D"));
        CHECK(u.pin("8Q").descriptor().name == std::string_view("8Q"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Rising edge samples D=High -> Q=High")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("1D").drive<Level>(Level::High);

        u.pin("CLK").drive<Level>(Level::Low);
        u.pin("CLK").drive<Level>(Level::High);

        CHECK(u.pin("1Q").read<Level>() == Level::High);
    }

    TEST_CASE("Rising edge samples D=Low -> Q=Low")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("1D").drive<Level>(Level::Low);

        u.pin("CLK").drive<Level>(Level::Low);
        u.pin("CLK").drive<Level>(Level::High);

        CHECK(u.pin("1Q").read<Level>() == Level::Low);
    }

    TEST_CASE("No rising edge: Q holds state")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);

        // sample High
        u.pin("1D").drive<Level>(Level::High);
        u.pin("CLK").drive<Level>(Level::Low);
        u.pin("CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        // CLK stays High -> no new sample
        u.pin("1D").drive<Level>(Level::Low);
        CHECK(u.pin("1Q").read<Level>() == Level::High);
    }

    TEST_CASE("OC=High forces outputs to HighZ")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("CLK").drive<Level>(Level::Low);
        u.pin("CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("OC").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Q").read<Level>() == Level::HighZ);
    }

    TEST_CASE("OC restored to Low after disable: outputs reflect sampled values")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("2D").drive<Level>(Level::Low);
        u.pin("CLK").drive<Level>(Level::Low);
        u.pin("CLK").drive<Level>(Level::High);

        u.pin("OC").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);

        u.pin("OC").drive<Level>(Level::Low);
        CHECK(u.pin("1Q").read<Level>() == Level::High);
        CHECK(u.pin("2Q").read<Level>() == Level::Low);
    }

    TEST_CASE("All eight FFs are independent")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);

        u.pin("1D").drive<Level>(Level::High);
        u.pin("2D").drive<Level>(Level::Low);
        u.pin("3D").drive<Level>(Level::High);
        u.pin("4D").drive<Level>(Level::Low);
        u.pin("5D").drive<Level>(Level::High);
        u.pin("6D").drive<Level>(Level::Low);
        u.pin("7D").drive<Level>(Level::High);
        u.pin("8D").drive<Level>(Level::Low);

        u.pin("CLK").drive<Level>(Level::Low);
        u.pin("CLK").drive<Level>(Level::High);

        CHECK(u.pin("1Q").read<Level>() == Level::High);
        CHECK(u.pin("2Q").read<Level>() == Level::Low);
        CHECK(u.pin("3Q").read<Level>() == Level::High);
        CHECK(u.pin("4Q").read<Level>() == Level::Low);
        CHECK(u.pin("5Q").read<Level>() == Level::High);
        CHECK(u.pin("6Q").read<Level>() == Level::Low);
        CHECK(u.pin("7Q").read<Level>() == Level::High);
        CHECK(u.pin("8Q").read<Level>() == Level::Low);
    }

    TEST_CASE("VCC removed -> outputs HighZ")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("CLK").drive<Level>(Level::Low);
        u.pin("CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("VCC").release();
        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
    }

    TEST_CASE("reset() drives all Q=Low")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("CLK").drive<Level>(Level::Low);
        u.pin("CLK").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.reset();
        CHECK(u.pin("1Q").read<Level>() == Level::Low);
        CHECK(u.pin("2Q").read<Level>() == Level::Low);
        CHECK(u.pin("3Q").read<Level>() == Level::Low);
        CHECK(u.pin("4Q").read<Level>() == Level::Low);
        CHECK(u.pin("5Q").read<Level>() == Level::Low);
        CHECK(u.pin("6Q").read<Level>() == Level::Low);
        CHECK(u.pin("7Q").read<Level>() == Level::Low);
        CHECK(u.pin("8Q").read<Level>() == Level::Low);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS374 u{ LS374::Descriptor{ .name = "U1" } };
        CHECK(u.name() == std::string_view("U1"));
    }
}
