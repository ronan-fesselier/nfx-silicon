#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::gate::sn74xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("gate::sn74xxx::LS00")
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

TEST_SUITE("gate::sn74xxx::LS04")
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

TEST_SUITE("gate::sn74xxx::LS08")
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

TEST_SUITE("gate::sn74xxx::LS32")
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

TEST_SUITE("gate::sn74xxx::LS86")
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
