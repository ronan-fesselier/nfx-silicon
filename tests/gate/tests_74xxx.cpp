#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::gate;
using namespace nfx::silicon::gate::sn74;

TEST_SUITE("gate::sn74::LS00")
{
    TEST_CASE("Construction creates 14 pins")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 14);
    }

    TEST_CASE("Pin names match DIP-14 datasheet")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };

        CHECK(u.pin("1A").descriptor().name == std::string_view("1A"));
        CHECK(u.pin("1B").descriptor().name == std::string_view("1B"));
        CHECK(u.pin("1Y").descriptor().name == std::string_view("1Y"));
        CHECK(u.pin("2A").descriptor().name == std::string_view("2A"));
        CHECK(u.pin("2B").descriptor().name == std::string_view("2B"));
        CHECK(u.pin("2Y").descriptor().name == std::string_view("2Y"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("3Y").descriptor().name == std::string_view("3Y"));
        CHECK(u.pin("3B").descriptor().name == std::string_view("3B"));
        CHECK(u.pin("3A").descriptor().name == std::string_view("3A"));
        CHECK(u.pin("4Y").descriptor().name == std::string_view("4Y"));
        CHECK(u.pin("4B").descriptor().name == std::string_view("4B"));
        CHECK(u.pin("4A").descriptor().name == std::string_view("4A"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(LS00::Terminal::A1) == &u.pin("1A"));
        CHECK(&u.pin(LS00::Terminal::B1) == &u.pin("1B"));
        CHECK(&u.pin(LS00::Terminal::Y1) == &u.pin("1Y"));
        CHECK(&u.pin(LS00::Terminal::A2) == &u.pin("2A"));
        CHECK(&u.pin(LS00::Terminal::B2) == &u.pin("2B"));
        CHECK(&u.pin(LS00::Terminal::Y2) == &u.pin("2Y"));
        CHECK(&u.pin(LS00::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS00::Terminal::Y3) == &u.pin("3Y"));
        CHECK(&u.pin(LS00::Terminal::B3) == &u.pin("3B"));
        CHECK(&u.pin(LS00::Terminal::A3) == &u.pin("3A"));
        CHECK(&u.pin(LS00::Terminal::Y4) == &u.pin("4Y"));
        CHECK(&u.pin(LS00::Terminal::B4) == &u.pin("4B"));
        CHECK(&u.pin(LS00::Terminal::A4) == &u.pin("4A"));
        CHECK(&u.pin(LS00::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC driven at 5V: A=High B=High -> Y=Low")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
    }

    TEST_CASE("A=Low B=High -> Y=High")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=High B=Low -> Y=High")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=Low B=Low -> Y=High")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=HighZ -> Y=HighZ")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("1A").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("All four gates are independent")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);
        u.pin("2A").drive<Level>(Level::Low);
        u.pin("2B").drive<Level>(Level::High);
        u.pin("3A").drive<Level>(Level::High);
        u.pin("3B").drive<Level>(Level::High);
        u.pin("4A").drive<Level>(Level::High);
        u.pin("4B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
        CHECK(u.pin("2Y").read<Level>() == Level::High);
        CHECK(u.pin("3Y").read<Level>() == Level::Low);
        CHECK(u.pin("4Y").read<Level>() == Level::High);
    }

    TEST_CASE("VCC removed after operation -> outputs return to HighZ")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);
        CHECK(u.pin("1Y").read<Level>() == Level::Low);

        u.pin("VCC").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("3Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("4Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC below range -> outputs are HighZ")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);
        CHECK(u.pin("1Y").read<Level>() == Level::Low);

        u.pin("VCC").drive<float>(3.0f);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS00 u{ LS00::Descriptor{ .name = "U1" } };

        CHECK(u.name() == std::string_view("U1"));
    }
}

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

TEST_SUITE("gate::sn74::LS08")
{
    TEST_CASE("Construction creates 14 pins")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 14);
    }

    TEST_CASE("Pin names match DIP-14 datasheet")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };

        CHECK(u.pin("1A").descriptor().name == std::string_view("1A"));
        CHECK(u.pin("1B").descriptor().name == std::string_view("1B"));
        CHECK(u.pin("1Y").descriptor().name == std::string_view("1Y"));
        CHECK(u.pin("2A").descriptor().name == std::string_view("2A"));
        CHECK(u.pin("2B").descriptor().name == std::string_view("2B"));
        CHECK(u.pin("2Y").descriptor().name == std::string_view("2Y"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("3Y").descriptor().name == std::string_view("3Y"));
        CHECK(u.pin("3A").descriptor().name == std::string_view("3A"));
        CHECK(u.pin("3B").descriptor().name == std::string_view("3B"));
        CHECK(u.pin("4Y").descriptor().name == std::string_view("4Y"));
        CHECK(u.pin("4A").descriptor().name == std::string_view("4A"));
        CHECK(u.pin("4B").descriptor().name == std::string_view("4B"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(LS08::Terminal::A1) == &u.pin("1A"));
        CHECK(&u.pin(LS08::Terminal::B1) == &u.pin("1B"));
        CHECK(&u.pin(LS08::Terminal::Y1) == &u.pin("1Y"));
        CHECK(&u.pin(LS08::Terminal::A2) == &u.pin("2A"));
        CHECK(&u.pin(LS08::Terminal::B2) == &u.pin("2B"));
        CHECK(&u.pin(LS08::Terminal::Y2) == &u.pin("2Y"));
        CHECK(&u.pin(LS08::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS08::Terminal::Y3) == &u.pin("3Y"));
        CHECK(&u.pin(LS08::Terminal::A3) == &u.pin("3A"));
        CHECK(&u.pin(LS08::Terminal::B3) == &u.pin("3B"));
        CHECK(&u.pin(LS08::Terminal::Y4) == &u.pin("4Y"));
        CHECK(&u.pin(LS08::Terminal::A4) == &u.pin("4A"));
        CHECK(&u.pin(LS08::Terminal::B4) == &u.pin("4B"));
        CHECK(&u.pin(LS08::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC driven at 5V: A=High B=High -> Y=High")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=Low B=High -> Y=Low")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
    }

    TEST_CASE("A=High B=Low -> Y=Low")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
    }

    TEST_CASE("A=Low B=Low -> Y=Low")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
    }

    TEST_CASE("A=HighZ -> Y=HighZ")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("1A").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("All four gates are independent")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);
        u.pin("2A").drive<Level>(Level::Low);
        u.pin("2B").drive<Level>(Level::High);
        u.pin("3A").drive<Level>(Level::High);
        u.pin("3B").drive<Level>(Level::High);
        u.pin("4A").drive<Level>(Level::High);
        u.pin("4B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
        CHECK(u.pin("2Y").read<Level>() == Level::Low);
        CHECK(u.pin("3Y").read<Level>() == Level::High);
        CHECK(u.pin("4Y").read<Level>() == Level::Low);
    }

    TEST_CASE("VCC removed after operation -> outputs return to HighZ")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("VCC").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("3Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("4Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC below range -> outputs are HighZ")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("VCC").drive<float>(3.0f);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS08 u{ LS08::Descriptor{ .name = "U1" } };

        CHECK(u.name() == std::string_view("U1"));
    }
}

TEST_SUITE("gate::sn74::LS32")
{
    TEST_CASE("Construction creates 14 pins")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 14);
    }

    TEST_CASE("Pin names match DIP-14 datasheet")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };

        CHECK(u.pin("1A").descriptor().name == std::string_view("1A"));
        CHECK(u.pin("1B").descriptor().name == std::string_view("1B"));
        CHECK(u.pin("1Y").descriptor().name == std::string_view("1Y"));
        CHECK(u.pin("2A").descriptor().name == std::string_view("2A"));
        CHECK(u.pin("2B").descriptor().name == std::string_view("2B"));
        CHECK(u.pin("2Y").descriptor().name == std::string_view("2Y"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("3Y").descriptor().name == std::string_view("3Y"));
        CHECK(u.pin("3A").descriptor().name == std::string_view("3A"));
        CHECK(u.pin("3B").descriptor().name == std::string_view("3B"));
        CHECK(u.pin("4Y").descriptor().name == std::string_view("4Y"));
        CHECK(u.pin("4A").descriptor().name == std::string_view("4A"));
        CHECK(u.pin("4B").descriptor().name == std::string_view("4B"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(LS32::Terminal::A1) == &u.pin("1A"));
        CHECK(&u.pin(LS32::Terminal::B1) == &u.pin("1B"));
        CHECK(&u.pin(LS32::Terminal::Y1) == &u.pin("1Y"));
        CHECK(&u.pin(LS32::Terminal::A2) == &u.pin("2A"));
        CHECK(&u.pin(LS32::Terminal::B2) == &u.pin("2B"));
        CHECK(&u.pin(LS32::Terminal::Y2) == &u.pin("2Y"));
        CHECK(&u.pin(LS32::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS32::Terminal::Y3) == &u.pin("3Y"));
        CHECK(&u.pin(LS32::Terminal::A3) == &u.pin("3A"));
        CHECK(&u.pin(LS32::Terminal::B3) == &u.pin("3B"));
        CHECK(&u.pin(LS32::Terminal::Y4) == &u.pin("4Y"));
        CHECK(&u.pin(LS32::Terminal::A4) == &u.pin("4A"));
        CHECK(&u.pin(LS32::Terminal::B4) == &u.pin("4B"));
        CHECK(&u.pin(LS32::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC driven at 5V: A=Low B=Low -> Y=Low")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
    }

    TEST_CASE("A=Low B=High -> Y=High")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=High B=Low -> Y=High")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=High B=High -> Y=High")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=HighZ -> Y=HighZ")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);
        CHECK(u.pin("1Y").read<Level>() == Level::Low);

        u.pin("1A").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("All four gates are independent")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);
        u.pin("2A").drive<Level>(Level::High);
        u.pin("2B").drive<Level>(Level::Low);
        u.pin("3A").drive<Level>(Level::Low);
        u.pin("3B").drive<Level>(Level::Low);
        u.pin("4A").drive<Level>(Level::High);
        u.pin("4B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
        CHECK(u.pin("2Y").read<Level>() == Level::High);
        CHECK(u.pin("3Y").read<Level>() == Level::Low);
        CHECK(u.pin("4Y").read<Level>() == Level::High);
    }

    TEST_CASE("VCC removed after operation -> outputs return to HighZ")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::Low);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("VCC").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("3Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("4Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC below range -> outputs are HighZ")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::Low);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("VCC").drive<float>(3.0f);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS32 u{ LS32::Descriptor{ .name = "U1" } };

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

TEST_SUITE("gate::sn74::LS86")
{
    TEST_CASE("Construction creates 14 pins")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 14);
    }

    TEST_CASE("Pin names match DIP-14 datasheet")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };

        CHECK(u.pin("1A").descriptor().name == std::string_view("1A"));
        CHECK(u.pin("1B").descriptor().name == std::string_view("1B"));
        CHECK(u.pin("1Y").descriptor().name == std::string_view("1Y"));
        CHECK(u.pin("2A").descriptor().name == std::string_view("2A"));
        CHECK(u.pin("2B").descriptor().name == std::string_view("2B"));
        CHECK(u.pin("2Y").descriptor().name == std::string_view("2Y"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("3Y").descriptor().name == std::string_view("3Y"));
        CHECK(u.pin("3A").descriptor().name == std::string_view("3A"));
        CHECK(u.pin("3B").descriptor().name == std::string_view("3B"));
        CHECK(u.pin("4Y").descriptor().name == std::string_view("4Y"));
        CHECK(u.pin("4A").descriptor().name == std::string_view("4A"));
        CHECK(u.pin("4B").descriptor().name == std::string_view("4B"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(LS86::Terminal::A1) == &u.pin("1A"));
        CHECK(&u.pin(LS86::Terminal::B1) == &u.pin("1B"));
        CHECK(&u.pin(LS86::Terminal::Y1) == &u.pin("1Y"));
        CHECK(&u.pin(LS86::Terminal::A2) == &u.pin("2A"));
        CHECK(&u.pin(LS86::Terminal::B2) == &u.pin("2B"));
        CHECK(&u.pin(LS86::Terminal::Y2) == &u.pin("2Y"));
        CHECK(&u.pin(LS86::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS86::Terminal::Y3) == &u.pin("3Y"));
        CHECK(&u.pin(LS86::Terminal::A3) == &u.pin("3A"));
        CHECK(&u.pin(LS86::Terminal::B3) == &u.pin("3B"));
        CHECK(&u.pin(LS86::Terminal::Y4) == &u.pin("4Y"));
        CHECK(&u.pin(LS86::Terminal::A4) == &u.pin("4A"));
        CHECK(&u.pin(LS86::Terminal::B4) == &u.pin("4B"));
        CHECK(&u.pin(LS86::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC driven at 5V: A=Low B=Low -> Y=Low")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
    }

    TEST_CASE("A=Low B=High -> Y=High")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=High B=Low -> Y=High")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::Low);

        CHECK(u.pin("1Y").read<Level>() == Level::High);
    }

    TEST_CASE("A=High B=High -> Y=Low")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::High);
        u.pin("1B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
    }

    TEST_CASE("A=HighZ -> Y=HighZ")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::High);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("1A").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("All four gates are independent")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::Low);
        u.pin("2A").drive<Level>(Level::Low);
        u.pin("2B").drive<Level>(Level::High);
        u.pin("3A").drive<Level>(Level::High);
        u.pin("3B").drive<Level>(Level::Low);
        u.pin("4A").drive<Level>(Level::High);
        u.pin("4B").drive<Level>(Level::High);

        CHECK(u.pin("1Y").read<Level>() == Level::Low);
        CHECK(u.pin("2Y").read<Level>() == Level::High);
        CHECK(u.pin("3Y").read<Level>() == Level::High);
        CHECK(u.pin("4Y").read<Level>() == Level::Low);
    }

    TEST_CASE("VCC removed after operation -> outputs return to HighZ")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::High);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("VCC").release();

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("2Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("3Y").read<Level>() == Level::HighZ);
        CHECK(u.pin("4Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC below range -> outputs are HighZ")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("1A").drive<Level>(Level::Low);
        u.pin("1B").drive<Level>(Level::High);
        CHECK(u.pin("1Y").read<Level>() == Level::High);

        u.pin("VCC").drive<float>(3.0f);

        CHECK(u.pin("1Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS86 u{ LS86::Descriptor{ .name = "U1" } };

        CHECK(u.name() == std::string_view("U1"));
    }
}

TEST_SUITE("gate::sn74::LS138")
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

TEST_SUITE("gate::sn74::LS139")
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

TEST_SUITE("gate::sn74::LS245")
{
    TEST_CASE("Construction creates 20 pins")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        CHECK(u.pins().size() == 20);
    }

    TEST_CASE("Pin names match DIP-20 datasheet")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        CHECK(u.pin("DIR").descriptor().name == std::string_view("DIR"));
        CHECK(u.pin("A1").descriptor().name == std::string_view("A1"));
        CHECK(u.pin("A2").descriptor().name == std::string_view("A2"));
        CHECK(u.pin("A3").descriptor().name == std::string_view("A3"));
        CHECK(u.pin("A4").descriptor().name == std::string_view("A4"));
        CHECK(u.pin("A5").descriptor().name == std::string_view("A5"));
        CHECK(u.pin("A6").descriptor().name == std::string_view("A6"));
        CHECK(u.pin("A7").descriptor().name == std::string_view("A7"));
        CHECK(u.pin("A8").descriptor().name == std::string_view("A8"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("B8").descriptor().name == std::string_view("B8"));
        CHECK(u.pin("B7").descriptor().name == std::string_view("B7"));
        CHECK(u.pin("B6").descriptor().name == std::string_view("B6"));
        CHECK(u.pin("B5").descriptor().name == std::string_view("B5"));
        CHECK(u.pin("B4").descriptor().name == std::string_view("B4"));
        CHECK(u.pin("B3").descriptor().name == std::string_view("B3"));
        CHECK(u.pin("B2").descriptor().name == std::string_view("B2"));
        CHECK(u.pin("B1").descriptor().name == std::string_view("B1"));
        CHECK(u.pin("/OE").descriptor().name == std::string_view("/OE"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        CHECK(&u.pin(LS245::Terminal::DIR) == &u.pin("DIR"));
        CHECK(&u.pin(LS245::Terminal::A1) == &u.pin("A1"));
        CHECK(&u.pin(LS245::Terminal::A2) == &u.pin("A2"));
        CHECK(&u.pin(LS245::Terminal::A3) == &u.pin("A3"));
        CHECK(&u.pin(LS245::Terminal::A4) == &u.pin("A4"));
        CHECK(&u.pin(LS245::Terminal::A5) == &u.pin("A5"));
        CHECK(&u.pin(LS245::Terminal::A6) == &u.pin("A6"));
        CHECK(&u.pin(LS245::Terminal::A7) == &u.pin("A7"));
        CHECK(&u.pin(LS245::Terminal::A8) == &u.pin("A8"));
        CHECK(&u.pin(LS245::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(LS245::Terminal::B8) == &u.pin("B8"));
        CHECK(&u.pin(LS245::Terminal::B7) == &u.pin("B7"));
        CHECK(&u.pin(LS245::Terminal::B6) == &u.pin("B6"));
        CHECK(&u.pin(LS245::Terminal::B5) == &u.pin("B5"));
        CHECK(&u.pin(LS245::Terminal::B4) == &u.pin("B4"));
        CHECK(&u.pin(LS245::Terminal::B3) == &u.pin("B3"));
        CHECK(&u.pin(LS245::Terminal::B2) == &u.pin("B2"));
        CHECK(&u.pin(LS245::Terminal::B1) == &u.pin("B1"));
        CHECK(&u.pin(LS245::Terminal::nOE) == &u.pin("/OE"));
        CHECK(&u.pin(LS245::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("VCC not driven: A and B are HighZ")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);
        src.drive<Level>(Level::High);

        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("OE=High: isolation, B is HighZ regardless of A")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        u.pin("DIR").drive<Level>(Level::High);
        u.pin("/OE").drive<Level>(Level::High);
        src.drive<Level>(Level::High);

        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("OE=HighZ: isolation")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        u.pin("DIR").drive<Level>(Level::High);
        src.drive<Level>(Level::High);

        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("DIR=High (A->B): A drives B")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        src.drive<Level>(Level::High);
        CHECK(u.pin("B1").read<Level>() == Level::High);

        src.drive<Level>(Level::Low);
        CHECK(u.pin("B1").read<Level>() == Level::Low);
    }

    TEST_CASE("DIR=Low (B->A): B drives A")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::Low);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wb{ Wire::Descriptor{ .name = "WB", .kind = Pin::Kind::Digital } };
        wb.attach(src);
        wb.attach(u.pin("B1"));

        Pin obs{ Pin::Descriptor{ .name = "OBS", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(u.pin("A1"));
        wa.attach(obs);

        src.drive<Level>(Level::High);
        CHECK(obs.read<Level>() == Level::High);

        src.drive<Level>(Level::Low);
        CHECK(obs.read<Level>() == Level::Low);
    }

    TEST_CASE("DIR change reverses data flow")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);

        Pin srcA{ Pin::Descriptor{ .name = "SRCA", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin srcB{ Pin::Descriptor{ .name = "SRCB", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin obsA{ Pin::Descriptor{ .name = "OBSA", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Pin obsB{ Pin::Descriptor{ .name = "OBSB", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };

        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        Wire wb{ Wire::Descriptor{ .name = "WB", .kind = Pin::Kind::Digital } };
        wa.attach(srcA);
        wa.attach(u.pin("A1"));
        wa.attach(obsA);
        wb.attach(srcB);
        wb.attach(u.pin("B1"));
        wb.attach(obsB);

        // DIR=High: A->B
        u.pin("DIR").drive<Level>(Level::High);
        srcA.drive<Level>(Level::High);
        CHECK(obsB.read<Level>() == Level::High);

        srcA.release();
        u.pin("DIR").drive<Level>(Level::Low);
        srcB.drive<Level>(Level::Low);
        CHECK(obsA.read<Level>() == Level::Low);
    }

    TEST_CASE("A input HighZ -> B output HighZ (DIR=High)")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);

        // A1 has no driver -> B1 should be HighZ
        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC removed after operation: outputs return to HighZ")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);

        Pin src{ Pin::Descriptor{ .name = "SRC", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wa{ Wire::Descriptor{ .name = "WA", .kind = Pin::Kind::Digital } };
        wa.attach(src);
        wa.attach(u.pin("A1"));

        src.drive<Level>(Level::High);
        CHECK(u.pin("B1").read<Level>() == Level::High);

        u.pin("VCC").release();
        CHECK(u.pin("B1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("All 8 channels propagate independently (DIR=High)")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("/OE").drive<Level>(Level::Low);
        u.pin("DIR").drive<Level>(Level::High);

        const char* aPins[8] = { "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8" };
        const char* bPins[8] = { "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8" };
        const Level levels[8] = { Level::High, Level::Low,  Level::High, Level::Low,
                                  Level::Low,  Level::High, Level::Low,  Level::High };

        Pin src[8] = {
            Pin{ Pin::Descriptor{ .name = "S0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S3", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S4", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S5", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S6", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } },
            Pin{ Pin::Descriptor{ .name = "S7", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } }
        };
        Wire wa[8] = { Wire{ Wire::Descriptor{ .name = "W0", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W1", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W2", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W3", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W4", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W5", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W6", .kind = Pin::Kind::Digital } },
                       Wire{ Wire::Descriptor{ .name = "W7", .kind = Pin::Kind::Digital } } };
        for (int i = 0; i < 8; ++i)
        {
            wa[i].attach(src[i]);
            wa[i].attach(u.pin(aPins[i]));
            src[i].drive<Level>(levels[i]);
        }
        for (int i = 0; i < 8; ++i)
        {
            CHECK(u.pin(bPins[i]).read<Level>() == levels[i]);
        }
    }

    TEST_CASE("Component name is stored correctly")
    {
        LS245 u{ LS245::Descriptor{ .name = "U1" } };
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
