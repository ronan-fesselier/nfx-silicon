#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::gate;
using namespace nfx::silicon::gate::sn74;

TEST_SUITE("gate::sn74::LS04")
{
    TEST_CASE("Construction creates 14 pins")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 14);
    }

    TEST_CASE("Pin names match DIP-14 datasheet")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };

        CHECK(u.pin("1A").descriptor().name == std::string_view("1A"));
        CHECK(u.pin("1Y").descriptor().name == std::string_view("1Y"));
        CHECK(u.pin("2A").descriptor().name == std::string_view("2A"));
        CHECK(u.pin("2Y").descriptor().name == std::string_view("2Y"));
        CHECK(u.pin("3A").descriptor().name == std::string_view("3A"));
        CHECK(u.pin("3Y").descriptor().name == std::string_view("3Y"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("4Y").descriptor().name == std::string_view("4Y"));
        CHECK(u.pin("4A").descriptor().name == std::string_view("4A"));
        CHECK(u.pin("5Y").descriptor().name == std::string_view("5Y"));
        CHECK(u.pin("5A").descriptor().name == std::string_view("5A"));
        CHECK(u.pin("6Y").descriptor().name == std::string_view("6Y"));
        CHECK(u.pin("6A").descriptor().name == std::string_view("6A"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(LS04::Terminal::A1) == &u.pin("1A"));
        CHECK(&u.pin(LS04::Terminal::Y1) == &u.pin("1Y"));
        CHECK(&u.pin(LS04::Terminal::A2) == &u.pin("2A"));
        CHECK(&u.pin(LS04::Terminal::Y2) == &u.pin("2Y"));
        CHECK(&u.pin(LS04::Terminal::A3) == &u.pin("3A"));
        CHECK(&u.pin(LS04::Terminal::Y3) == &u.pin("3Y"));
        CHECK(&u.pin(LS04::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS04::Terminal::Y4) == &u.pin("4Y"));
        CHECK(&u.pin(LS04::Terminal::A4) == &u.pin("4A"));
        CHECK(&u.pin(LS04::Terminal::Y5) == &u.pin("5Y"));
        CHECK(&u.pin(LS04::Terminal::A5) == &u.pin("5A"));
        CHECK(&u.pin(LS04::Terminal::Y6) == &u.pin("6Y"));
        CHECK(&u.pin(LS04::Terminal::A6) == &u.pin("6A"));
        CHECK(&u.pin(LS04::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };
        u.pin("1A").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC driven at 5V: A=Low -> Y=High")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("VCC driven at 5V: A=High -> Y=Low")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
    }

    TEST_CASE("A=HighZ -> Y=HighZ")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        CHECK(u.pin("1Y").read<Level>() == Level::Low);

        u.pin("1A").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("All six inverters are independent")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("2A").drive<Level>(Level::High);
        u.pin("3A").drive<Level>(Level::Low);
        u.pin("4A").drive<Level>(Level::High);
        u.pin("5A").drive<Level>(Level::Low);
        u.pin("6A").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
        CHECK(u.pin("2Y").read<Level>() == Level::Low);
        CHECK(u.pin("3Y").read<Level>() == Level::High);
        CHECK(u.pin("4Y").read<Level>() == Level::Low);
        CHECK(u.pin("5Y").read<Level>() == Level::High);
        CHECK(u.pin("6Y").read<Level>() == Level::Low);
    }

    TEST_CASE("VCC removed after operation -> outputs return to HighZ")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("VCC").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("3Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("4Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("5Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("6Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC below range -> outputs are HighZ")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("VCC").drive<float>(3.0f);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS04 u{ LS04::Descriptor{ .name = "U1" } };

        CHECK(u.name() == std::string_view("U1"));
    }
}

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

TEST_SUITE("gate::sn74::LS373")
{
    TEST_CASE("Construction creates 20 pins")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 20);
    }

    TEST_CASE("Pin names match DIP-20 datasheet")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };

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

        CHECK(u.pin("C").descriptor().name == std::string_view("C"));
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

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(LS373::Terminal::OC) == &u.pin("OC"));
        CHECK(&u.pin(LS373::Terminal::Q1) == &u.pin("1Q"));
        CHECK(&u.pin(LS373::Terminal::D1) == &u.pin("1D"));
        CHECK(&u.pin(LS373::Terminal::D2) == &u.pin("2D"));
        CHECK(&u.pin(LS373::Terminal::Q2) == &u.pin("2Q"));
        CHECK(&u.pin(LS373::Terminal::Q3) == &u.pin("3Q"));
        CHECK(&u.pin(LS373::Terminal::D3) == &u.pin("3D"));
        CHECK(&u.pin(LS373::Terminal::D4) == &u.pin("4D"));
        CHECK(&u.pin(LS373::Terminal::Q4) == &u.pin("4Q"));
        CHECK(&u.pin(LS373::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS373::Terminal::C) == &u.pin("C"));
        CHECK(&u.pin(LS373::Terminal::Q5) == &u.pin("5Q"));
        CHECK(&u.pin(LS373::Terminal::D5) == &u.pin("5D"));
        CHECK(&u.pin(LS373::Terminal::D6) == &u.pin("6D"));
        CHECK(&u.pin(LS373::Terminal::Q6) == &u.pin("6Q"));
        CHECK(&u.pin(LS373::Terminal::Q7) == &u.pin("7Q"));
        CHECK(&u.pin(LS373::Terminal::D7) == &u.pin("7D"));
        CHECK(&u.pin(LS373::Terminal::D8) == &u.pin("8D"));
        CHECK(&u.pin(LS373::Terminal::Q8) == &u.pin("8Q"));
        CHECK(&u.pin(LS373::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC driven at 5V: transparent mode Q1 follows D1")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::High);

        u.pin("1D").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("1D").drive<Level>(Level::Low);
        CHECK(u.pin("1Q").read<Level>() == Level::Low);
    }

    TEST_CASE("C=Low latches D: output holds after input changes")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);

        u.pin("C").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("C").drive<Level>(Level::Low);

        u.pin("1D").drive<Level>(Level::Low);
        CHECK(u.pin("1Q").read<Level>() == Level::High);
    }

    TEST_CASE("OC=High forces all outputs to HighZ regardless of latch state")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("OC").drive<Level>(Level::High);

        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Q").read<Level>() == Level::HighZ);
    }

    TEST_CASE("OC restored to Low after disable: outputs reflect latched values")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("C").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("2D").drive<Level>(Level::Low);

        u.pin("OC").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);

        u.pin("OC").drive<Level>(Level::Low);
        CHECK(u.pin("1Q").read<Level>() == Level::High);
        CHECK(u.pin("2Q").read<Level>() == Level::Low);
    }

    TEST_CASE("VCC removed after operation -> outputs return to HighZ")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("VCC").release();

        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Q").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC restored after power loss -> latches start from reset state")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::High);

        u.pin("VCC").drive<float>(5.0f);
        u.pin("1D").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("VCC").release();
        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);

        u.pin("VCC").drive<float>(5.0f);

        CHECK(u.pin("1Q").read<Level>() == Level::Low);
    }

    TEST_CASE("VCC below range -> outputs are HighZ")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::High);
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1D").drive<Level>(Level::High);
        CHECK(u.pin("1Q").read<Level>() == Level::High);

        u.pin("VCC").drive<float>(2.0f);

        CHECK(u.pin("1Q").read<Level>() == Level::HighZ);
    }

    TEST_CASE("All eight latches are independent")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::High);

        u.pin("1D").drive<Level>(Level::High);
        u.pin("2D").drive<Level>(Level::Low);
        u.pin("3D").drive<Level>(Level::High);
        u.pin("4D").drive<Level>(Level::Low);
        u.pin("5D").drive<Level>(Level::High);
        u.pin("6D").drive<Level>(Level::Low);
        u.pin("7D").drive<Level>(Level::High);
        u.pin("8D").drive<Level>(Level::Low);

        CHECK(u.pin("1Q").read<Level>() == Level::High);
        CHECK(u.pin("2Q").read<Level>() == Level::Low);
        CHECK(u.pin("3Q").read<Level>() == Level::High);
        CHECK(u.pin("4Q").read<Level>() == Level::Low);
        CHECK(u.pin("5Q").read<Level>() == Level::High);
        CHECK(u.pin("6Q").read<Level>() == Level::Low);
        CHECK(u.pin("7Q").read<Level>() == Level::High);
        CHECK(u.pin("8Q").read<Level>() == Level::Low);
    }

    TEST_CASE("reset() drives all Q outputs to Low")
    {
        LS373 u{ LS373::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("OC").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::High);
        u.pin("1D").drive<Level>(Level::High);
        u.pin("8D").drive<Level>(Level::High);
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
        LS373 u{ LS373::Descriptor{ .name = "U1" } };

        CHECK(u.name() == std::string_view("U1"));
    }
}

TEST_SUITE("gate::sn74::LS374")
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
