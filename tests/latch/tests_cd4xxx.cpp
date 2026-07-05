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

TEST_SUITE("latch::cd4xxx::CD4508B")
{
    TEST_CASE("Construction creates 24 pins")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };

        CHECK(u.pins().size() == 24);
    }

    TEST_CASE("Pin names match DIP-24 datasheet")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };

        CHECK(u.pin("RESA").descriptor().name == std::string_view("RESA"));
        CHECK(u.pin("STRBA").descriptor().name == std::string_view("STRBA"));
        CHECK(u.pin("DISA").descriptor().name == std::string_view("DISA"));
        CHECK(u.pin("D0A").descriptor().name == std::string_view("D0A"));
        CHECK(u.pin("Q0A").descriptor().name == std::string_view("Q0A"));
        CHECK(u.pin("D1A").descriptor().name == std::string_view("D1A"));
        CHECK(u.pin("Q1A").descriptor().name == std::string_view("Q1A"));
        CHECK(u.pin("D2A").descriptor().name == std::string_view("D2A"));
        CHECK(u.pin("Q2A").descriptor().name == std::string_view("Q2A"));
        CHECK(u.pin("D3A").descriptor().name == std::string_view("D3A"));
        CHECK(u.pin("Q3A").descriptor().name == std::string_view("Q3A"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("RESB").descriptor().name == std::string_view("RESB"));
        CHECK(u.pin("STRBB").descriptor().name == std::string_view("STRBB"));
        CHECK(u.pin("DISB").descriptor().name == std::string_view("DISB"));
        CHECK(u.pin("D0B").descriptor().name == std::string_view("D0B"));
        CHECK(u.pin("Q0B").descriptor().name == std::string_view("Q0B"));
        CHECK(u.pin("D1B").descriptor().name == std::string_view("D1B"));
        CHECK(u.pin("Q1B").descriptor().name == std::string_view("Q1B"));
        CHECK(u.pin("D2B").descriptor().name == std::string_view("D2B"));
        CHECK(u.pin("Q2B").descriptor().name == std::string_view("Q2B"));
        CHECK(u.pin("D3B").descriptor().name == std::string_view("D3B"));
        CHECK(u.pin("Q3B").descriptor().name == std::string_view("Q3B"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };

        CHECK(&u.pin(CD4508B::Terminal::RESA) == &u.pin("RESA"));
        CHECK(&u.pin(CD4508B::Terminal::STRBA) == &u.pin("STRBA"));
        CHECK(&u.pin(CD4508B::Terminal::DISA) == &u.pin("DISA"));
        CHECK(&u.pin(CD4508B::Terminal::D0A) == &u.pin("D0A"));
        CHECK(&u.pin(CD4508B::Terminal::Q0A) == &u.pin("Q0A"));
        CHECK(&u.pin(CD4508B::Terminal::D1A) == &u.pin("D1A"));
        CHECK(&u.pin(CD4508B::Terminal::Q1A) == &u.pin("Q1A"));
        CHECK(&u.pin(CD4508B::Terminal::D2A) == &u.pin("D2A"));
        CHECK(&u.pin(CD4508B::Terminal::Q2A) == &u.pin("Q2A"));
        CHECK(&u.pin(CD4508B::Terminal::D3A) == &u.pin("D3A"));
        CHECK(&u.pin(CD4508B::Terminal::Q3A) == &u.pin("Q3A"));
        CHECK(&u.pin(CD4508B::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CD4508B::Terminal::RESB) == &u.pin("RESB"));
        CHECK(&u.pin(CD4508B::Terminal::STRBB) == &u.pin("STRBB"));
        CHECK(&u.pin(CD4508B::Terminal::DISB) == &u.pin("DISB"));
        CHECK(&u.pin(CD4508B::Terminal::D0B) == &u.pin("D0B"));
        CHECK(&u.pin(CD4508B::Terminal::Q0B) == &u.pin("Q0B"));
        CHECK(&u.pin(CD4508B::Terminal::D1B) == &u.pin("D1B"));
        CHECK(&u.pin(CD4508B::Terminal::Q1B) == &u.pin("Q1B"));
        CHECK(&u.pin(CD4508B::Terminal::D2B) == &u.pin("D2B"));
        CHECK(&u.pin(CD4508B::Terminal::Q2B) == &u.pin("Q2B"));
        CHECK(&u.pin(CD4508B::Terminal::D3B) == &u.pin("D3B"));
        CHECK(&u.pin(CD4508B::Terminal::Q3B) == &u.pin("Q3B"));
        CHECK(&u.pin(CD4508B::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("VDD not driven: outputs are HighZ")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("STRBA").drive<Level>(Level::High);
        u.pin("DISA").drive<Level>(Level::Low);
        u.pin("RESA").drive<Level>(Level::Low);
        u.pin("D0A").drive<Level>(Level::High);

        CHECK(u.pin("Q0A").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD below range: outputs are HighZ")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("STRBA").drive<Level>(Level::High);
        u.pin("DISA").drive<Level>(Level::Low);
        u.pin("RESA").drive<Level>(Level::Low);
        u.pin("D0A").drive<Level>(Level::High);
        u.pin("D1A").drive<Level>(Level::Low);
        u.pin("D2A").drive<Level>(Level::Low);
        u.pin("D3A").drive<Level>(Level::Low);
        u.pin("VDD").drive<float>(5.0f);
        CHECK(u.pin("Q0A").read<Level>() == Level::High);

        u.pin("VDD").drive<float>(1.0f);

        CHECK(u.pin("Q0A").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD above range: outputs are HighZ")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("STRBA").drive<Level>(Level::High);
        u.pin("DISA").drive<Level>(Level::Low);
        u.pin("RESA").drive<Level>(Level::Low);
        u.pin("D0A").drive<Level>(Level::High);
        u.pin("D1A").drive<Level>(Level::Low);
        u.pin("D2A").drive<Level>(Level::Low);
        u.pin("D3A").drive<Level>(Level::Low);
        u.pin("VDD").drive<float>(5.0f);
        CHECK(u.pin("Q0A").read<Level>() == Level::High);

        u.pin("VDD").drive<float>(20.0f);

        CHECK(u.pin("Q0A").read<Level>() == Level::HighZ);
    }

    TEST_CASE("DISABLE=High: outputs are HighZ")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("DISA").drive<Level>(Level::High);
        u.pin("RESA").drive<Level>(Level::Low);
        u.pin("STRBA").drive<Level>(Level::High);
        u.pin("D0A").drive<Level>(Level::High);
        u.pin("D1A").drive<Level>(Level::High);
        u.pin("D2A").drive<Level>(Level::High);
        u.pin("D3A").drive<Level>(Level::High);

        CHECK(u.pin("Q0A").read<Level>() == Level::HighZ);
        CHECK(u.pin("Q3A").read<Level>() == Level::HighZ);
    }

    TEST_CASE("RESET=High: all outputs Low")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("DISA").drive<Level>(Level::Low);
        u.pin("RESA").drive<Level>(Level::High);
        u.pin("STRBA").drive<Level>(Level::High);
        u.pin("D0A").drive<Level>(Level::High);
        u.pin("D1A").drive<Level>(Level::High);
        u.pin("D2A").drive<Level>(Level::High);
        u.pin("D3A").drive<Level>(Level::High);

        CHECK(u.pin("Q0A").read<Level>() == Level::Low);
        CHECK(u.pin("Q1A").read<Level>() == Level::Low);
        CHECK(u.pin("Q2A").read<Level>() == Level::Low);
        CHECK(u.pin("Q3A").read<Level>() == Level::Low);
    }

    TEST_CASE("STROBE=High: transparent mode tracks D inputs")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("DISA").drive<Level>(Level::Low);
        u.pin("RESA").drive<Level>(Level::Low);
        u.pin("STRBA").drive<Level>(Level::High);

        u.pin("D0A").drive<Level>(Level::High);
        u.pin("D1A").drive<Level>(Level::Low);
        u.pin("D2A").drive<Level>(Level::High);
        u.pin("D3A").drive<Level>(Level::Low);

        CHECK(u.pin("Q0A").read<Level>() == Level::High);
        CHECK(u.pin("Q1A").read<Level>() == Level::Low);
        CHECK(u.pin("Q2A").read<Level>() == Level::High);
        CHECK(u.pin("Q3A").read<Level>() == Level::Low);
    }

    TEST_CASE("STROBE=Low: outputs latch and ignore D changes")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("DISA").drive<Level>(Level::Low);
        u.pin("RESA").drive<Level>(Level::Low);
        u.pin("STRBA").drive<Level>(Level::High);
        u.pin("D0A").drive<Level>(Level::High);
        u.pin("D1A").drive<Level>(Level::Low);
        u.pin("D2A").drive<Level>(Level::High);
        u.pin("D3A").drive<Level>(Level::Low);

        u.pin("STRBA").drive<Level>(Level::Low);
        u.pin("D0A").drive<Level>(Level::Low);
        u.pin("D1A").drive<Level>(Level::High);

        CHECK(u.pin("Q0A").read<Level>() == Level::High);
        CHECK(u.pin("Q1A").read<Level>() == Level::Low);
        CHECK(u.pin("Q2A").read<Level>() == Level::High);
        CHECK(u.pin("Q3A").read<Level>() == Level::Low);
    }

    TEST_CASE("Two latches are independent")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("DISA").drive<Level>(Level::Low);
        u.pin("RESA").drive<Level>(Level::Low);
        u.pin("STRBA").drive<Level>(Level::High);
        u.pin("DISB").drive<Level>(Level::Low);
        u.pin("RESB").drive<Level>(Level::Low);
        u.pin("STRBB").drive<Level>(Level::High);

        u.pin("D0A").drive<Level>(Level::High);
        u.pin("D1A").drive<Level>(Level::Low);
        u.pin("D2A").drive<Level>(Level::Low);
        u.pin("D3A").drive<Level>(Level::Low);
        u.pin("D0B").drive<Level>(Level::Low);
        u.pin("D1B").drive<Level>(Level::Low);
        u.pin("D2B").drive<Level>(Level::Low);
        u.pin("D3B").drive<Level>(Level::Low);

        CHECK(u.pin("Q0A").read<Level>() == Level::High);
        CHECK(u.pin("Q0B").read<Level>() == Level::Low);
    }

    TEST_CASE("reset() drives all Q outputs Low")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("VDD").drive<float>(5.0f);
        u.pin("DISA").drive<Level>(Level::Low);
        u.pin("RESA").drive<Level>(Level::Low);
        u.pin("STRBA").drive<Level>(Level::High);
        u.pin("DISB").drive<Level>(Level::Low);
        u.pin("RESB").drive<Level>(Level::Low);
        u.pin("STRBB").drive<Level>(Level::High);
        u.pin("D0A").drive<Level>(Level::High);
        u.pin("D0B").drive<Level>(Level::High);

        u.reset();

        CHECK(u.pin("Q0A").read<Level>() == Level::Low);
        CHECK(u.pin("Q3A").read<Level>() == Level::Low);
        CHECK(u.pin("Q0B").read<Level>() == Level::Low);
        CHECK(u.pin("Q3B").read<Level>() == Level::Low);
    }

    TEST_CASE("VDD removed after operation: outputs return to HighZ")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };
        u.pin("DISA").drive<Level>(Level::Low);
        u.pin("RESA").drive<Level>(Level::Low);
        u.pin("STRBA").drive<Level>(Level::High);
        u.pin("D0A").drive<Level>(Level::High);
        u.pin("D1A").drive<Level>(Level::Low);
        u.pin("D2A").drive<Level>(Level::Low);
        u.pin("D3A").drive<Level>(Level::Low);
        u.pin("VDD").drive<float>(5.0f);
        CHECK(u.pin("Q0A").read<Level>() == Level::High);

        u.pin("VDD").release();

        CHECK(u.pin("Q0A").read<Level>() == Level::HighZ);
        CHECK(u.pin("Q0B").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        CD4508B u{ CD4508B::Descriptor{ .name = "U24" } };

        CHECK(u.name() == std::string_view("U24"));
    }
}
