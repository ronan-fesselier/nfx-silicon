#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::flipflop::cd4xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("flipflop::cd4xxx::CD4013B")
{
    TEST_CASE("Construction creates 14 pins")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 14);
    }

    TEST_CASE("Pin names match DIP-14 datasheet (TI CD4013B)")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };

        CHECK(u.pin("Q1").descriptor().name == std::string_view("Q1"));
        CHECK(u.pin("/Q1").descriptor().name == std::string_view("/Q1"));
        CHECK(u.pin("CLOCK1").descriptor().name == std::string_view("CLOCK1"));
        CHECK(u.pin("RESET1").descriptor().name == std::string_view("RESET1"));
        CHECK(u.pin("D1").descriptor().name == std::string_view("D1"));
        CHECK(u.pin("SET1").descriptor().name == std::string_view("SET1"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("SET2").descriptor().name == std::string_view("SET2"));
        CHECK(u.pin("D2").descriptor().name == std::string_view("D2"));
        CHECK(u.pin("RESET2").descriptor().name == std::string_view("RESET2"));
        CHECK(u.pin("CLOCK2").descriptor().name == std::string_view("CLOCK2"));
        CHECK(u.pin("/Q2").descriptor().name == std::string_view("/Q2"));
        CHECK(u.pin("Q2").descriptor().name == std::string_view("Q2"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(CD4013B::Terminal::Q1) == &u.pin("Q1"));
        CHECK(&u.pin(CD4013B::Terminal::nQ1) == &u.pin("/Q1"));
        CHECK(&u.pin(CD4013B::Terminal::CLOCK1) == &u.pin("CLOCK1"));
        CHECK(&u.pin(CD4013B::Terminal::RESET1) == &u.pin("RESET1"));
        CHECK(&u.pin(CD4013B::Terminal::D1) == &u.pin("D1"));
        CHECK(&u.pin(CD4013B::Terminal::SET1) == &u.pin("SET1"));
        CHECK(&u.pin(CD4013B::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CD4013B::Terminal::SET2) == &u.pin("SET2"));
        CHECK(&u.pin(CD4013B::Terminal::D2) == &u.pin("D2"));
        CHECK(&u.pin(CD4013B::Terminal::RESET2) == &u.pin("RESET2"));
        CHECK(&u.pin(CD4013B::Terminal::CLOCK2) == &u.pin("CLOCK2"));
        CHECK(&u.pin(CD4013B::Terminal::nQ2) == &u.pin("/Q2"));
        CHECK(&u.pin(CD4013B::Terminal::Q2) == &u.pin("Q2"));
        CHECK(&u.pin(CD4013B::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("VDD not driven: outputs are HighZ")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("D1").drive<Level>(Level::High);

        CHECK(u.pin("Q1").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("FF1 async set (SET1=High): Q1=High /Q1=Low")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("RESET1").drive<Level>(Level::Low);
        u.pin("SET1").drive<Level>(Level::High);

        CHECK(u.pin("Q1").read<Level>() == Level::High);
        CHECK(u.pin("/Q1").read<Level>() == Level::Low);
    }

    TEST_CASE("FF1 async reset (RESET1=High): Q1=Low /Q1=High")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("SET1").drive<Level>(Level::Low);
        u.pin("RESET1").drive<Level>(Level::High);

        CHECK(u.pin("Q1").read<Level>() == Level::Low);
        CHECK(u.pin("/Q1").read<Level>() == Level::High);
    }

    TEST_CASE("FF1 forbidden state (SET1=High RESET1=High): Q1=HighZ /Q1=HighZ")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("SET1").drive<Level>(Level::High);
        u.pin("RESET1").drive<Level>(Level::High);

        CHECK(u.pin("Q1").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("FF1 clock capture D=High on rising edge: Q1=High")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("SET1").drive<Level>(Level::Low);
        u.pin("RESET1").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::High);
        u.pin("CLOCK1").drive<Level>(Level::Low);
        u.pin("CLOCK1").drive<Level>(Level::High);

        CHECK(u.pin("Q1").read<Level>() == Level::High);
        CHECK(u.pin("/Q1").read<Level>() == Level::Low);
    }

    TEST_CASE("FF1 clock capture D=Low on rising edge: Q1=Low")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("SET1").drive<Level>(Level::Low);
        u.pin("RESET1").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::Low);
        u.pin("CLOCK1").drive<Level>(Level::Low);
        u.pin("CLOCK1").drive<Level>(Level::High);

        CHECK(u.pin("Q1").read<Level>() == Level::Low);
        CHECK(u.pin("/Q1").read<Level>() == Level::High);
    }

    TEST_CASE("FF1 hold: D change after clock edge does not affect output")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("SET1").drive<Level>(Level::Low);
        u.pin("RESET1").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::High);
        u.pin("CLOCK1").drive<Level>(Level::Low);
        u.pin("CLOCK1").drive<Level>(Level::High);
        CHECK(u.pin("Q1").read<Level>() == Level::High);

        u.pin("D1").drive<Level>(Level::Low);
        CHECK(u.pin("Q1").read<Level>() == Level::High);
    }

    TEST_CASE("FF1 and FF2 are independent")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("SET1").drive<Level>(Level::Low);
        u.pin("RESET1").drive<Level>(Level::Low);
        u.pin("SET2").drive<Level>(Level::Low);
        u.pin("RESET2").drive<Level>(Level::Low);

        u.pin("D1").drive<Level>(Level::High);
        u.pin("D2").drive<Level>(Level::Low);
        u.pin("CLOCK1").drive<Level>(Level::Low);
        u.pin("CLOCK1").drive<Level>(Level::High);
        u.pin("CLOCK2").drive<Level>(Level::Low);
        u.pin("CLOCK2").drive<Level>(Level::High);

        CHECK(u.pin("Q1").read<Level>() == Level::High);
        CHECK(u.pin("Q2").read<Level>() == Level::Low);
    }

    TEST_CASE("reset() drives Q=Low /Q=High on both flip-flops")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("SET1").drive<Level>(Level::High);
        u.pin("SET2").drive<Level>(Level::High);
        u.pin("RESET1").drive<Level>(Level::Low);
        u.pin("RESET2").drive<Level>(Level::Low);
        CHECK(u.pin("Q1").read<Level>() == Level::High);
        CHECK(u.pin("Q2").read<Level>() == Level::High);

        u.reset();

        CHECK(u.pin("Q1").read<Level>() == Level::Low);
        CHECK(u.pin("/Q1").read<Level>() == Level::High);
        CHECK(u.pin("Q2").read<Level>() == Level::Low);
        CHECK(u.pin("/Q2").read<Level>() == Level::High);
    }

    TEST_CASE("VDD removal releases all outputs to HighZ")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("SET1").drive<Level>(Level::Low);
        u.pin("RESET1").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::High);
        u.pin("CLOCK1").drive<Level>(Level::Low);
        u.pin("CLOCK1").drive<Level>(Level::High);
        CHECK(u.pin("Q1").read<Level>() == Level::High);

        u.pin("VDD").release();

        CHECK(u.pin("Q1").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q1").read<Level>() == Level::HighZ);
        CHECK(u.pin("Q2").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q2").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD below range: outputs are HighZ")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("SET1").drive<Level>(Level::Low);
        u.pin("RESET1").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::High);
        u.pin("CLOCK1").drive<Level>(Level::Low);
        u.pin("CLOCK1").drive<Level>(Level::High);
        CHECK(u.pin("Q1").read<Level>() == Level::High);

        u.pin("VDD").drive<float>(2.0f);

        CHECK(u.pin("Q1").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name")
    {
        CD4013B u{ CD4013B::Descriptor{ .name = "U7" } };

        CHECK(u.name() == std::string_view("U7"));
    }
}
