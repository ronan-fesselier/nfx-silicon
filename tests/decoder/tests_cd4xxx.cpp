#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::decoder::cd4xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("decoder::cd4xxx::CD4028B")
{
    TEST_CASE("Construction creates 16 pins")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };

        CHECK(u.pins().size() == 16);
    }

    TEST_CASE("Pin names match DIP-16 datasheet")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };

        CHECK(u.pin("Y4").descriptor().name == std::string_view("Y4"));
        CHECK(u.pin("Y2").descriptor().name == std::string_view("Y2"));
        CHECK(u.pin("Y0").descriptor().name == std::string_view("Y0"));
        CHECK(u.pin("Y7").descriptor().name == std::string_view("Y7"));
        CHECK(u.pin("Y9").descriptor().name == std::string_view("Y9"));
        CHECK(u.pin("Y5").descriptor().name == std::string_view("Y5"));
        CHECK(u.pin("Y6").descriptor().name == std::string_view("Y6"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("Y8").descriptor().name == std::string_view("Y8"));
        CHECK(u.pin("A").descriptor().name == std::string_view("A"));
        CHECK(u.pin("D").descriptor().name == std::string_view("D"));
        CHECK(u.pin("C").descriptor().name == std::string_view("C"));
        CHECK(u.pin("B").descriptor().name == std::string_view("B"));
        CHECK(u.pin("Y1").descriptor().name == std::string_view("Y1"));
        CHECK(u.pin("Y3").descriptor().name == std::string_view("Y3"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };

        CHECK(&u.pin(CD4028B::Terminal::Y4) == &u.pin("Y4"));
        CHECK(&u.pin(CD4028B::Terminal::Y2) == &u.pin("Y2"));
        CHECK(&u.pin(CD4028B::Terminal::Y0) == &u.pin("Y0"));
        CHECK(&u.pin(CD4028B::Terminal::Y7) == &u.pin("Y7"));
        CHECK(&u.pin(CD4028B::Terminal::Y9) == &u.pin("Y9"));
        CHECK(&u.pin(CD4028B::Terminal::Y5) == &u.pin("Y5"));
        CHECK(&u.pin(CD4028B::Terminal::Y6) == &u.pin("Y6"));
        CHECK(&u.pin(CD4028B::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CD4028B::Terminal::Y8) == &u.pin("Y8"));
        CHECK(&u.pin(CD4028B::Terminal::A) == &u.pin("A"));
        CHECK(&u.pin(CD4028B::Terminal::D) == &u.pin("D"));
        CHECK(&u.pin(CD4028B::Terminal::C) == &u.pin("C"));
        CHECK(&u.pin(CD4028B::Terminal::B) == &u.pin("B"));
        CHECK(&u.pin(CD4028B::Terminal::Y1) == &u.pin("Y1"));
        CHECK(&u.pin(CD4028B::Terminal::Y3) == &u.pin("Y3"));
        CHECK(&u.pin(CD4028B::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("VDD not driven: outputs are HighZ")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        u.pin("D").drive<Level>(Level::Low);

        CHECK(u.pin("Y0").read<Level>() == Level::HighZ);
        CHECK(u.pin("Y9").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD below range: outputs are HighZ")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        u.pin("D").drive<Level>(Level::Low);
        CHECK(u.pin("Y0").read<Level>() == Level::High);

        u.pin("VDD").drive<float>(1.0f);

        CHECK(u.pin("Y0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD above range: outputs are HighZ")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        u.pin("D").drive<Level>(Level::Low);
        CHECK(u.pin("Y0").read<Level>() == Level::High);

        u.pin("VDD").drive<float>(21.0f);

        CHECK(u.pin("Y0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("BCD codes 0-9: correct output High, all others Low")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };
        u.pin("VDD").drive<float>(5.0f);

        const char* yPins[10] = { "Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7", "Y8", "Y9" };

        for (std::uint8_t code = 0; code < 10; ++code)
        {
            u.pin("A").drive<Level>(code & 1u ? Level::High : Level::Low);
            u.pin("B").drive<Level>(code & 2u ? Level::High : Level::Low);
            u.pin("C").drive<Level>(code & 4u ? Level::High : Level::Low);
            u.pin("D").drive<Level>(code & 8u ? Level::High : Level::Low);

            for (std::uint8_t i = 0; i < 10; ++i)
            {
                CHECK(u.pin(yPins[i]).read<Level>() == (i == code ? Level::High : Level::Low));
            }
        }
    }

    TEST_CASE("Binary-to-octal mode (D=Low): CBA selects Y0..Y7")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("D").drive<Level>(Level::Low);

        const char* yPins[8] = { "Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7" };

        for (std::uint8_t code = 0; code < 8; ++code)
        {
            u.pin("A").drive<Level>(code & 1u ? Level::High : Level::Low);
            u.pin("B").drive<Level>(code & 2u ? Level::High : Level::Low);
            u.pin("C").drive<Level>(code & 4u ? Level::High : Level::Low);

            for (std::uint8_t i = 0; i < 8; ++i)
            {
                CHECK(u.pin(yPins[i]).read<Level>() == (i == code ? Level::High : Level::Low));
            }
            CHECK(u.pin("Y8").read<Level>() == Level::Low);
            CHECK(u.pin("Y9").read<Level>() == Level::Low);
        }
    }

    TEST_CASE("Invalid BCD codes 10-15: all outputs Low")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };
        u.pin("VDD").drive<float>(5.0f);

        const char* yPins[10] = { "Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7", "Y8", "Y9" };

        for (std::uint8_t code = 10; code < 16; ++code)
        {
            u.pin("A").drive<Level>(code & 1u ? Level::High : Level::Low);
            u.pin("B").drive<Level>(code & 2u ? Level::High : Level::Low);
            u.pin("C").drive<Level>(code & 4u ? Level::High : Level::Low);
            u.pin("D").drive<Level>(code & 8u ? Level::High : Level::Low);

            for (std::uint8_t i = 0; i < 10; ++i)
            {
                CHECK(u.pin(yPins[i]).read<Level>() == Level::Low);
            }
        }
    }

    TEST_CASE("Any input HighZ: all outputs HighZ")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        u.pin("D").drive<Level>(Level::Low);
        CHECK(u.pin("Y0").read<Level>() == Level::High);

        u.pin("B").release();

        CHECK(u.pin("Y0").read<Level>() == Level::HighZ);
        CHECK(u.pin("Y9").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD removed after operation: outputs return to HighZ")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        u.pin("D").drive<Level>(Level::Low);
        CHECK(u.pin("Y0").read<Level>() == Level::High);

        u.pin("VDD").release();

        CHECK(u.pin("Y0").read<Level>() == Level::HighZ);
        CHECK(u.pin("Y9").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        CD4028B u{ CD4028B::Descriptor{ .name = "U12" } };

        CHECK(u.name() == std::string_view("U12"));
    }
}

TEST_SUITE("decoder::cd4xxx::CD4556B")
{
    TEST_CASE("Construction creates 16 pins")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };

        CHECK(u.pins().size() == 16);
    }

    TEST_CASE("Pin names match DIP-16 datasheet")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };

        CHECK(u.pin("/E1").descriptor().name == std::string_view("/E1"));
        CHECK(u.pin("A1").descriptor().name == std::string_view("A1"));
        CHECK(u.pin("B1").descriptor().name == std::string_view("B1"));
        CHECK(u.pin("/1Q0").descriptor().name == std::string_view("/1Q0"));
        CHECK(u.pin("/1Q1").descriptor().name == std::string_view("/1Q1"));
        CHECK(u.pin("/1Q2").descriptor().name == std::string_view("/1Q2"));
        CHECK(u.pin("/1Q3").descriptor().name == std::string_view("/1Q3"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("/2Q3").descriptor().name == std::string_view("/2Q3"));
        CHECK(u.pin("/2Q2").descriptor().name == std::string_view("/2Q2"));
        CHECK(u.pin("/2Q1").descriptor().name == std::string_view("/2Q1"));
        CHECK(u.pin("/2Q0").descriptor().name == std::string_view("/2Q0"));
        CHECK(u.pin("B2").descriptor().name == std::string_view("B2"));
        CHECK(u.pin("A2").descriptor().name == std::string_view("A2"));
        CHECK(u.pin("/E2").descriptor().name == std::string_view("/E2"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };

        CHECK(&u.pin(CD4556B::Terminal::nE1) == &u.pin("/E1"));
        CHECK(&u.pin(CD4556B::Terminal::A1) == &u.pin("A1"));
        CHECK(&u.pin(CD4556B::Terminal::B1) == &u.pin("B1"));
        CHECK(&u.pin(CD4556B::Terminal::nQ1_0) == &u.pin("/1Q0"));
        CHECK(&u.pin(CD4556B::Terminal::nQ1_1) == &u.pin("/1Q1"));
        CHECK(&u.pin(CD4556B::Terminal::nQ1_2) == &u.pin("/1Q2"));
        CHECK(&u.pin(CD4556B::Terminal::nQ1_3) == &u.pin("/1Q3"));
        CHECK(&u.pin(CD4556B::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CD4556B::Terminal::nQ2_3) == &u.pin("/2Q3"));
        CHECK(&u.pin(CD4556B::Terminal::nQ2_2) == &u.pin("/2Q2"));
        CHECK(&u.pin(CD4556B::Terminal::nQ2_1) == &u.pin("/2Q1"));
        CHECK(&u.pin(CD4556B::Terminal::nQ2_0) == &u.pin("/2Q0"));
        CHECK(&u.pin(CD4556B::Terminal::B2) == &u.pin("B2"));
        CHECK(&u.pin(CD4556B::Terminal::A2) == &u.pin("A2"));
        CHECK(&u.pin(CD4556B::Terminal::nE2) == &u.pin("/E2"));
        CHECK(&u.pin(CD4556B::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("VDD not driven: outputs are HighZ")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("/E1").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("B1").drive<Level>(Level::Low);

        CHECK(u.pin("/1Q0").read<Level>() == Level::HighZ);
        CHECK(u.pin("/1Q3").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD below range: outputs are HighZ")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("/E1").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("B1").drive<Level>(Level::Low);
        CHECK(u.pin("/1Q0").read<Level>() == Level::Low);

        u.pin("VDD").drive<float>(1.0f);

        CHECK(u.pin("/1Q0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD above range: outputs are HighZ")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("/E1").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("B1").drive<Level>(Level::Low);
        CHECK(u.pin("/1Q0").read<Level>() == Level::Low);

        u.pin("VDD").drive<float>(20.0f);

        CHECK(u.pin("/1Q0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Decoder 1 disabled (/E1=High): all outputs High")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("/E1").drive<Level>(Level::High);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("B1").drive<Level>(Level::Low);

        CHECK(u.pin("/1Q0").read<Level>() == Level::High);
        CHECK(u.pin("/1Q1").read<Level>() == Level::High);
        CHECK(u.pin("/1Q2").read<Level>() == Level::High);
        CHECK(u.pin("/1Q3").read<Level>() == Level::High);
    }

    TEST_CASE("Decoder 1 enabled: all 4 addresses decode correctly")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("/E1").drive<Level>(Level::Low);

        const char* qPins[4] = { "/1Q0", "/1Q1", "/1Q2", "/1Q3" };

        for (std::uint8_t addr = 0; addr < 4; ++addr)
        {
            u.pin("A1").drive<Level>(addr & 1u ? Level::High : Level::Low);
            u.pin("B1").drive<Level>(addr & 2u ? Level::High : Level::Low);

            for (std::uint8_t i = 0; i < 4; ++i)
            {
                CHECK(u.pin(qPins[i]).read<Level>() == (i == addr ? Level::Low : Level::High));
            }
        }
    }

    TEST_CASE("Decoder 2 disabled (/E2=High): all outputs High")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("/E2").drive<Level>(Level::High);
        u.pin("A2").drive<Level>(Level::Low);
        u.pin("B2").drive<Level>(Level::Low);

        CHECK(u.pin("/2Q0").read<Level>() == Level::High);
        CHECK(u.pin("/2Q1").read<Level>() == Level::High);
        CHECK(u.pin("/2Q2").read<Level>() == Level::High);
        CHECK(u.pin("/2Q3").read<Level>() == Level::High);
    }

    TEST_CASE("Decoder 2 enabled: all 4 addresses decode correctly")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("/E2").drive<Level>(Level::Low);

        const char* qPins[4] = { "/2Q0", "/2Q1", "/2Q2", "/2Q3" };

        for (std::uint8_t addr = 0; addr < 4; ++addr)
        {
            u.pin("A2").drive<Level>(addr & 1u ? Level::High : Level::Low);
            u.pin("B2").drive<Level>(addr & 2u ? Level::High : Level::Low);

            for (std::uint8_t i = 0; i < 4; ++i)
            {
                CHECK(u.pin(qPins[i]).read<Level>() == (i == addr ? Level::Low : Level::High));
            }
        }
    }

    TEST_CASE("Two decoders are independent")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("/E1").drive<Level>(Level::Low);
        u.pin("/E2").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("B1").drive<Level>(Level::Low);
        u.pin("A2").drive<Level>(Level::High);
        u.pin("B2").drive<Level>(Level::High);

        CHECK(u.pin("/1Q0").read<Level>() == Level::Low);
        CHECK(u.pin("/1Q1").read<Level>() == Level::High);
        CHECK(u.pin("/2Q3").read<Level>() == Level::Low);
        CHECK(u.pin("/2Q0").read<Level>() == Level::High);
    }

    TEST_CASE("Any input HighZ: outputs HighZ")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("/E1").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("B1").drive<Level>(Level::Low);
        CHECK(u.pin("/1Q0").read<Level>() == Level::Low);

        u.pin("A1").release();

        CHECK(u.pin("/1Q0").read<Level>() == Level::HighZ);
        CHECK(u.pin("/1Q3").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD removed after operation: outputs return to HighZ")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("/E1").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("B1").drive<Level>(Level::Low);
        CHECK(u.pin("/1Q0").read<Level>() == Level::Low);

        u.pin("VDD").release();

        CHECK(u.pin("/1Q0").read<Level>() == Level::HighZ);
        CHECK(u.pin("/2Q0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        CD4556B u{ CD4556B::Descriptor{ .name = "U9" } };

        CHECK(u.name() == std::string_view("U9"));
    }
}
