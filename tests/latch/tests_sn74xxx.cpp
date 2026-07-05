#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::latch::sn74xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("latch::sn74xxx::LS373")
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
