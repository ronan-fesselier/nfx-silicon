#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::buffer::cd4xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("buffer::cd4xxx::CD4049UB")
{
    TEST_CASE("Construction creates 14 pins")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 14);
    }

    TEST_CASE("Pin names match DIP-16 datasheet (TI CD4049UB)")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };

        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
        CHECK(u.pin("G").descriptor().name == std::string_view("G"));
        CHECK(u.pin("A").descriptor().name == std::string_view("A"));
        CHECK(u.pin("H").descriptor().name == std::string_view("H"));
        CHECK(u.pin("B").descriptor().name == std::string_view("B"));
        CHECK(u.pin("I").descriptor().name == std::string_view("I"));
        CHECK(u.pin("C").descriptor().name == std::string_view("C"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("D").descriptor().name == std::string_view("D"));
        CHECK(u.pin("J").descriptor().name == std::string_view("J"));
        CHECK(u.pin("E").descriptor().name == std::string_view("E"));
        CHECK(u.pin("K").descriptor().name == std::string_view("K"));
        CHECK(u.pin("F").descriptor().name == std::string_view("F"));
        CHECK(u.pin("L").descriptor().name == std::string_view("L"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(CD4049UB::Terminal::VCC) == &u.pin("VCC"));
        CHECK(&u.pin(CD4049UB::Terminal::G) == &u.pin("G"));
        CHECK(&u.pin(CD4049UB::Terminal::A) == &u.pin("A"));
        CHECK(&u.pin(CD4049UB::Terminal::H) == &u.pin("H"));
        CHECK(&u.pin(CD4049UB::Terminal::B) == &u.pin("B"));
        CHECK(&u.pin(CD4049UB::Terminal::I) == &u.pin("I"));
        CHECK(&u.pin(CD4049UB::Terminal::C) == &u.pin("C"));
        CHECK(&u.pin(CD4049UB::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CD4049UB::Terminal::D) == &u.pin("D"));
        CHECK(&u.pin(CD4049UB::Terminal::J) == &u.pin("J"));
        CHECK(&u.pin(CD4049UB::Terminal::E) == &u.pin("E"));
        CHECK(&u.pin(CD4049UB::Terminal::K) == &u.pin("K"));
        CHECK(&u.pin(CD4049UB::Terminal::F) == &u.pin("F"));
        CHECK(&u.pin(CD4049UB::Terminal::L) == &u.pin("L"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("A").drive<Level>(Level::High);

        CHECK(u.pin("G").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Gate 1 (A->G): High -> Low")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::High);

        CHECK(u.pin("G").read<Level>() == Level::Low);
    }

    TEST_CASE("Gate 1 (A->G): Low -> High")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);

        CHECK(u.pin("G").read<Level>() == Level::High);
    }

    TEST_CASE("Gate 1 (A->G): HighZ -> HighZ")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);
        CHECK(u.pin("G").read<Level>() == Level::High);

        u.pin("A").release();
        CHECK(u.pin("G").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Gate 2 (B->H): full truth table")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("B").drive<Level>(Level::High);
        CHECK(u.pin("H").read<Level>() == Level::Low);

        u.pin("B").drive<Level>(Level::Low);
        CHECK(u.pin("H").read<Level>() == Level::High);
    }

    TEST_CASE("Gate 3 (C->I): full truth table")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("C").drive<Level>(Level::High);
        CHECK(u.pin("I").read<Level>() == Level::Low);

        u.pin("C").drive<Level>(Level::Low);
        CHECK(u.pin("I").read<Level>() == Level::High);
    }

    TEST_CASE("Gate 4 (D->J): full truth table")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("D").drive<Level>(Level::High);
        CHECK(u.pin("J").read<Level>() == Level::Low);

        u.pin("D").drive<Level>(Level::Low);
        CHECK(u.pin("J").read<Level>() == Level::High);
    }

    TEST_CASE("Gate 5 (E->K): full truth table")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("E").drive<Level>(Level::High);
        CHECK(u.pin("K").read<Level>() == Level::Low);

        u.pin("E").drive<Level>(Level::Low);
        CHECK(u.pin("K").read<Level>() == Level::High);
    }

    TEST_CASE("Gate 6 (F->L): full truth table")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("F").drive<Level>(Level::High);
        CHECK(u.pin("L").read<Level>() == Level::Low);

        u.pin("F").drive<Level>(Level::Low);
        CHECK(u.pin("L").read<Level>() == Level::High);
    }

    TEST_CASE("Gates are independent")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("A").drive<Level>(Level::High);
        u.pin("B").drive<Level>(Level::Low);

        CHECK(u.pin("G").read<Level>() == Level::Low);
        CHECK(u.pin("H").read<Level>() == Level::High);
    }

    TEST_CASE("VCC removal releases all outputs to HighZ")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);
        u.pin("B").drive<Level>(Level::Low);
        u.pin("C").drive<Level>(Level::Low);
        u.pin("D").drive<Level>(Level::Low);
        u.pin("E").drive<Level>(Level::Low);
        u.pin("F").drive<Level>(Level::Low);

        CHECK(u.pin("G").read<Level>() == Level::High);
        CHECK(u.pin("H").read<Level>() == Level::High);
        CHECK(u.pin("I").read<Level>() == Level::High);
        CHECK(u.pin("J").read<Level>() == Level::High);
        CHECK(u.pin("K").read<Level>() == Level::High);
        CHECK(u.pin("L").read<Level>() == Level::High);

        u.pin("VCC").release();

        CHECK(u.pin("G").read<Level>() == Level::HighZ);
        CHECK(u.pin("H").read<Level>() == Level::HighZ);
        CHECK(u.pin("I").read<Level>() == Level::HighZ);
        CHECK(u.pin("J").read<Level>() == Level::HighZ);
        CHECK(u.pin("K").read<Level>() == Level::HighZ);
        CHECK(u.pin("L").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC below range: outputs are HighZ")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);
        CHECK(u.pin("G").read<Level>() == Level::High);

        u.pin("VCC").drive<float>(2.0f);
        CHECK(u.pin("G").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name")
    {
        CD4049UB u{ CD4049UB::Descriptor{ .name = "U4" } };

        CHECK(u.name() == std::string_view("U4"));
    }
}

TEST_SUITE("buffer::cd4xxx::CD4050B")
{
    TEST_CASE("Construction creates 14 pins")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 14);
    }

    TEST_CASE("Pin names match DIP-16 datasheet (TI CD4050B)")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };

        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
        CHECK(u.pin("G").descriptor().name == std::string_view("G"));
        CHECK(u.pin("A").descriptor().name == std::string_view("A"));
        CHECK(u.pin("H").descriptor().name == std::string_view("H"));
        CHECK(u.pin("B").descriptor().name == std::string_view("B"));
        CHECK(u.pin("I").descriptor().name == std::string_view("I"));
        CHECK(u.pin("C").descriptor().name == std::string_view("C"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("D").descriptor().name == std::string_view("D"));
        CHECK(u.pin("J").descriptor().name == std::string_view("J"));
        CHECK(u.pin("E").descriptor().name == std::string_view("E"));
        CHECK(u.pin("K").descriptor().name == std::string_view("K"));
        CHECK(u.pin("F").descriptor().name == std::string_view("F"));
        CHECK(u.pin("L").descriptor().name == std::string_view("L"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(CD4050B::Terminal::VCC) == &u.pin("VCC"));
        CHECK(&u.pin(CD4050B::Terminal::G) == &u.pin("G"));
        CHECK(&u.pin(CD4050B::Terminal::A) == &u.pin("A"));
        CHECK(&u.pin(CD4050B::Terminal::H) == &u.pin("H"));
        CHECK(&u.pin(CD4050B::Terminal::B) == &u.pin("B"));
        CHECK(&u.pin(CD4050B::Terminal::I) == &u.pin("I"));
        CHECK(&u.pin(CD4050B::Terminal::C) == &u.pin("C"));
        CHECK(&u.pin(CD4050B::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CD4050B::Terminal::D) == &u.pin("D"));
        CHECK(&u.pin(CD4050B::Terminal::J) == &u.pin("J"));
        CHECK(&u.pin(CD4050B::Terminal::E) == &u.pin("E"));
        CHECK(&u.pin(CD4050B::Terminal::K) == &u.pin("K"));
        CHECK(&u.pin(CD4050B::Terminal::F) == &u.pin("F"));
        CHECK(&u.pin(CD4050B::Terminal::L) == &u.pin("L"));
    }

    TEST_CASE("VCC not driven: outputs are HighZ")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("A").drive<Level>(Level::High);

        CHECK(u.pin("G").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Buffer 1 (A->G): High -> High")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::High);

        CHECK(u.pin("G").read<Level>() == Level::High);
    }

    TEST_CASE("Buffer 1 (A->G): Low -> Low")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);

        CHECK(u.pin("G").read<Level>() == Level::Low);
    }

    TEST_CASE("Buffer 1 (A->G): HighZ -> HighZ")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::Low);
        CHECK(u.pin("G").read<Level>() == Level::Low);

        u.pin("A").release();
        CHECK(u.pin("G").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Buffer 2 (B->H): full truth table")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("B").drive<Level>(Level::High);
        CHECK(u.pin("H").read<Level>() == Level::High);

        u.pin("B").drive<Level>(Level::Low);
        CHECK(u.pin("H").read<Level>() == Level::Low);
    }

    TEST_CASE("Buffer 3 (C->I): full truth table")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("C").drive<Level>(Level::High);
        CHECK(u.pin("I").read<Level>() == Level::High);

        u.pin("C").drive<Level>(Level::Low);
        CHECK(u.pin("I").read<Level>() == Level::Low);
    }

    TEST_CASE("Buffer 4 (D->J): full truth table")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("D").drive<Level>(Level::High);
        CHECK(u.pin("J").read<Level>() == Level::High);

        u.pin("D").drive<Level>(Level::Low);
        CHECK(u.pin("J").read<Level>() == Level::Low);
    }

    TEST_CASE("Buffer 5 (E->K): full truth table")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("E").drive<Level>(Level::High);
        CHECK(u.pin("K").read<Level>() == Level::High);

        u.pin("E").drive<Level>(Level::Low);
        CHECK(u.pin("K").read<Level>() == Level::Low);
    }

    TEST_CASE("Buffer 6 (F->L): full truth table")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("F").drive<Level>(Level::High);
        CHECK(u.pin("L").read<Level>() == Level::High);

        u.pin("F").drive<Level>(Level::Low);
        CHECK(u.pin("L").read<Level>() == Level::Low);
    }

    TEST_CASE("Buffers are independent")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);

        u.pin("A").drive<Level>(Level::High);
        u.pin("B").drive<Level>(Level::Low);

        CHECK(u.pin("G").read<Level>() == Level::High);
        CHECK(u.pin("H").read<Level>() == Level::Low);
    }

    TEST_CASE("VCC removal releases all outputs to HighZ")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::High);
        u.pin("B").drive<Level>(Level::High);
        u.pin("C").drive<Level>(Level::High);
        u.pin("D").drive<Level>(Level::High);
        u.pin("E").drive<Level>(Level::High);
        u.pin("F").drive<Level>(Level::High);

        CHECK(u.pin("G").read<Level>() == Level::High);
        CHECK(u.pin("H").read<Level>() == Level::High);
        CHECK(u.pin("I").read<Level>() == Level::High);
        CHECK(u.pin("J").read<Level>() == Level::High);
        CHECK(u.pin("K").read<Level>() == Level::High);
        CHECK(u.pin("L").read<Level>() == Level::High);

        u.pin("VCC").release();

        CHECK(u.pin("G").read<Level>() == Level::HighZ);
        CHECK(u.pin("H").read<Level>() == Level::HighZ);
        CHECK(u.pin("I").read<Level>() == Level::HighZ);
        CHECK(u.pin("J").read<Level>() == Level::HighZ);
        CHECK(u.pin("K").read<Level>() == Level::HighZ);
        CHECK(u.pin("L").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VCC below range: outputs are HighZ")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U1" } };
        u.pin("VCC").drive<float>(5.0f);
        u.pin("A").drive<Level>(Level::High);
        CHECK(u.pin("G").read<Level>() == Level::High);

        u.pin("VCC").drive<float>(2.0f);
        CHECK(u.pin("G").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name")
    {
        CD4050B u{ CD4050B::Descriptor{ .name = "U5" } };

        CHECK(u.name() == std::string_view("U5"));
    }
}
