#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::mux::cd4xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("mux::cd4xxx::CD4051B")
{
    TEST_CASE("Construction creates 16 pins")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };

        CHECK(u.pins().size() == 16);
    }

    TEST_CASE("Pin names match DIP-16 datasheet")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };

        CHECK(u.pin("CH4").descriptor().name == std::string_view("CH4"));
        CHECK(u.pin("CH6").descriptor().name == std::string_view("CH6"));
        CHECK(u.pin("COM").descriptor().name == std::string_view("COM"));
        CHECK(u.pin("CH7").descriptor().name == std::string_view("CH7"));
        CHECK(u.pin("CH5").descriptor().name == std::string_view("CH5"));
        CHECK(u.pin("INH").descriptor().name == std::string_view("INH"));
        CHECK(u.pin("VEE").descriptor().name == std::string_view("VEE"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("C").descriptor().name == std::string_view("C"));
        CHECK(u.pin("B").descriptor().name == std::string_view("B"));
        CHECK(u.pin("A").descriptor().name == std::string_view("A"));
        CHECK(u.pin("CH3").descriptor().name == std::string_view("CH3"));
        CHECK(u.pin("CH0").descriptor().name == std::string_view("CH0"));
        CHECK(u.pin("CH1").descriptor().name == std::string_view("CH1"));
        CHECK(u.pin("CH2").descriptor().name == std::string_view("CH2"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };

        CHECK(&u.pin(CD4051B::Terminal::CH4) == &u.pin("CH4"));
        CHECK(&u.pin(CD4051B::Terminal::CH6) == &u.pin("CH6"));
        CHECK(&u.pin(CD4051B::Terminal::COM) == &u.pin("COM"));
        CHECK(&u.pin(CD4051B::Terminal::CH7) == &u.pin("CH7"));
        CHECK(&u.pin(CD4051B::Terminal::CH5) == &u.pin("CH5"));
        CHECK(&u.pin(CD4051B::Terminal::INH) == &u.pin("INH"));
        CHECK(&u.pin(CD4051B::Terminal::VEE) == &u.pin("VEE"));
        CHECK(&u.pin(CD4051B::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CD4051B::Terminal::C) == &u.pin("C"));
        CHECK(&u.pin(CD4051B::Terminal::B) == &u.pin("B"));
        CHECK(&u.pin(CD4051B::Terminal::A) == &u.pin("A"));
        CHECK(&u.pin(CD4051B::Terminal::CH3) == &u.pin("CH3"));
        CHECK(&u.pin(CD4051B::Terminal::CH0) == &u.pin("CH0"));
        CHECK(&u.pin(CD4051B::Terminal::CH1) == &u.pin("CH1"));
        CHECK(&u.pin(CD4051B::Terminal::CH2) == &u.pin("CH2"));
        CHECK(&u.pin(CD4051B::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("VDD not driven: activeChannel returns nullopt")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };
        u.pin("INH").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);

        CHECK(u.activeChannel() == std::nullopt);
    }

    TEST_CASE("VDD below range: activeChannel returns nullopt")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };
        u.pin("VDD").drive<float>(1.0f);
        u.pin("INH").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);

        CHECK(u.activeChannel() == std::nullopt);
    }

    TEST_CASE("VDD above range: activeChannel returns nullopt")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };
        u.pin("VDD").drive<float>(21.0f);
        u.pin("INH").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);

        CHECK(u.activeChannel() == std::nullopt);
    }

    TEST_CASE("INH=High: activeChannel returns nullopt")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("INH").drive<Level>(Level::High);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);

        CHECK(u.activeChannel() == std::nullopt);
    }

    TEST_CASE("Any address input HighZ: activeChannel returns nullopt")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("INH").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);

        CHECK(u.activeChannel() == std::nullopt);
    }

    TEST_CASE("All 8 addresses select correct channel")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("INH").drive<Level>(Level::Low);

        for (std::uint8_t sel = 0; sel < 8; ++sel)
        {
            u.pin("A").drive<Level>(sel & 1u ? Level::High : Level::Low);
            u.pin("B").drive<Level>(sel & 2u ? Level::High : Level::Low);
            u.pin("C").drive<Level>(sel & 4u ? Level::High : Level::Low);

            CHECK(u.activeChannel().has_value());
            CHECK(u.activeChannel().value() == sel);
        }
    }

    TEST_CASE("INH returning Low re-enables channel selection")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);

        u.pin("INH").drive<Level>(Level::High);
        CHECK(u.activeChannel() == std::nullopt);

        u.pin("INH").drive<Level>(Level::Low);
        CHECK(u.activeChannel().has_value());
        CHECK(u.activeChannel().value() == 0);
    }

    TEST_CASE("VDD removed after operation: activeChannel returns nullopt")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("INH").drive<Level>(Level::Low);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        CHECK(u.activeChannel().value() == 0);

        u.pin("VDD").release();

        CHECK(u.activeChannel() == std::nullopt);
    }

    TEST_CASE("Component name is stored correctly")
    {
        CD4051B u{ CD4051B::Descriptor{ .name = "U11" } };

        CHECK(u.name() == std::string_view("U11"));
    }
}
