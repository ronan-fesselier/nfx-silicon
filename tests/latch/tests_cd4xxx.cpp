#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::latch::cd4xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("latch::cd4xxx::CD4042B")
{
    TEST_CASE("Construction creates 16 pins")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 16);
    }

    TEST_CASE("Pin names match DIP-16 datasheet (TI CD4042B)")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };

        CHECK(u.pin("Q4").descriptor().name == std::string_view("Q4"));
        CHECK(u.pin("Q1").descriptor().name == std::string_view("Q1"));
        CHECK(u.pin("/Q1").descriptor().name == std::string_view("/Q1"));
        CHECK(u.pin("D1").descriptor().name == std::string_view("D1"));
        CHECK(u.pin("CLOCK").descriptor().name == std::string_view("CLOCK"));
        CHECK(u.pin("POLARITY").descriptor().name == std::string_view("POLARITY"));
        CHECK(u.pin("D2").descriptor().name == std::string_view("D2"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("/Q2").descriptor().name == std::string_view("/Q2"));
        CHECK(u.pin("Q2").descriptor().name == std::string_view("Q2"));
        CHECK(u.pin("Q3").descriptor().name == std::string_view("Q3"));
        CHECK(u.pin("/Q3").descriptor().name == std::string_view("/Q3"));
        CHECK(u.pin("D3").descriptor().name == std::string_view("D3"));
        CHECK(u.pin("D4").descriptor().name == std::string_view("D4"));
        CHECK(u.pin("/Q4").descriptor().name == std::string_view("/Q4"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(CD4042B::Terminal::Q4) == &u.pin("Q4"));
        CHECK(&u.pin(CD4042B::Terminal::Q1) == &u.pin("Q1"));
        CHECK(&u.pin(CD4042B::Terminal::nQ1) == &u.pin("/Q1"));
        CHECK(&u.pin(CD4042B::Terminal::D1) == &u.pin("D1"));
        CHECK(&u.pin(CD4042B::Terminal::CLOCK) == &u.pin("CLOCK"));
        CHECK(&u.pin(CD4042B::Terminal::POLARITY) == &u.pin("POLARITY"));
        CHECK(&u.pin(CD4042B::Terminal::D2) == &u.pin("D2"));
        CHECK(&u.pin(CD4042B::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CD4042B::Terminal::nQ2) == &u.pin("/Q2"));
        CHECK(&u.pin(CD4042B::Terminal::Q2) == &u.pin("Q2"));
        CHECK(&u.pin(CD4042B::Terminal::Q3) == &u.pin("Q3"));
        CHECK(&u.pin(CD4042B::Terminal::nQ3) == &u.pin("/Q3"));
        CHECK(&u.pin(CD4042B::Terminal::D3) == &u.pin("D3"));
        CHECK(&u.pin(CD4042B::Terminal::D4) == &u.pin("D4"));
        CHECK(&u.pin(CD4042B::Terminal::nQ4) == &u.pin("/Q4"));
        CHECK(&u.pin(CD4042B::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("VDD not driven: outputs are HighZ")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };
        u.pin("D1").drive<Level>(Level::High);

        CHECK(u.pin("Q1").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("POLARITY=0 CLOCK=Low: transparent (Q follows D)")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("POLARITY").drive<Level>(Level::Low);
        u.pin("CLOCK").drive<Level>(Level::Low);

        u.pin("D1").drive<Level>(Level::High);
        CHECK(u.pin("Q1").read<Level>() == Level::High);
        CHECK(u.pin("/Q1").read<Level>() == Level::Low);

        u.pin("D1").drive<Level>(Level::Low);
        CHECK(u.pin("Q1").read<Level>() == Level::Low);
        CHECK(u.pin("/Q1").read<Level>() == Level::High);
    }

    TEST_CASE("POLARITY=0 CLOCK=High: latched (Q holds)")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("POLARITY").drive<Level>(Level::Low);
        u.pin("CLOCK").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::High);
        CHECK(u.pin("Q1").read<Level>() == Level::High);

        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("D1").drive<Level>(Level::Low);
        CHECK(u.pin("Q1").read<Level>() == Level::High);
    }

    TEST_CASE("POLARITY=1 CLOCK=High: transparent (Q follows D)")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("POLARITY").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);

        u.pin("D1").drive<Level>(Level::High);
        CHECK(u.pin("Q1").read<Level>() == Level::High);
        CHECK(u.pin("/Q1").read<Level>() == Level::Low);

        u.pin("D1").drive<Level>(Level::Low);
        CHECK(u.pin("Q1").read<Level>() == Level::Low);
        CHECK(u.pin("/Q1").read<Level>() == Level::High);
    }

    TEST_CASE("POLARITY=1 CLOCK=Low: latched (Q holds)")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("POLARITY").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("D1").drive<Level>(Level::High);
        CHECK(u.pin("Q1").read<Level>() == Level::High);

        u.pin("CLOCK").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::Low);
        CHECK(u.pin("Q1").read<Level>() == Level::High);
    }

    TEST_CASE("All four latches are independent")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("POLARITY").drive<Level>(Level::Low);
        u.pin("CLOCK").drive<Level>(Level::Low);

        u.pin("D1").drive<Level>(Level::High);
        u.pin("D2").drive<Level>(Level::Low);
        u.pin("D3").drive<Level>(Level::High);
        u.pin("D4").drive<Level>(Level::Low);

        CHECK(u.pin("Q1").read<Level>() == Level::High);
        CHECK(u.pin("Q2").read<Level>() == Level::Low);
        CHECK(u.pin("Q3").read<Level>() == Level::High);
        CHECK(u.pin("Q4").read<Level>() == Level::Low);
    }

    TEST_CASE("reset() drives Q=Low /Q=High on all latches")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("POLARITY").drive<Level>(Level::Low);
        u.pin("CLOCK").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::High);
        u.pin("D2").drive<Level>(Level::High);
        u.pin("D3").drive<Level>(Level::High);
        u.pin("D4").drive<Level>(Level::High);

        u.reset();

        CHECK(u.pin("Q1").read<Level>() == Level::Low);
        CHECK(u.pin("/Q1").read<Level>() == Level::High);
        CHECK(u.pin("Q2").read<Level>() == Level::Low);
        CHECK(u.pin("/Q2").read<Level>() == Level::High);
        CHECK(u.pin("Q3").read<Level>() == Level::Low);
        CHECK(u.pin("/Q3").read<Level>() == Level::High);
        CHECK(u.pin("Q4").read<Level>() == Level::Low);
        CHECK(u.pin("/Q4").read<Level>() == Level::High);
    }

    TEST_CASE("VDD removal releases all outputs to HighZ")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("POLARITY").drive<Level>(Level::Low);
        u.pin("CLOCK").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::High);
        u.pin("D2").drive<Level>(Level::High);
        u.pin("D3").drive<Level>(Level::High);
        u.pin("D4").drive<Level>(Level::High);

        u.pin("VDD").release();

        CHECK(u.pin("Q1").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q1").read<Level>() == Level::HighZ);
        CHECK(u.pin("Q2").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q2").read<Level>() == Level::HighZ);
        CHECK(u.pin("Q3").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q3").read<Level>() == Level::HighZ);
        CHECK(u.pin("Q4").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q4").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD below range: outputs are HighZ")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U1" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("POLARITY").drive<Level>(Level::Low);
        u.pin("CLOCK").drive<Level>(Level::Low);
        u.pin("D1").drive<Level>(Level::High);
        CHECK(u.pin("Q1").read<Level>() == Level::High);

        u.pin("VDD").drive<float>(2.0f);

        CHECK(u.pin("Q1").read<Level>() == Level::HighZ);
        CHECK(u.pin("/Q1").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name")
    {
        CD4042B u{ CD4042B::Descriptor{ .name = "U8" } };

        CHECK(u.name() == std::string_view("U8"));
    }
}
