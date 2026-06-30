#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::discrete;

TEST_SUITE("discrete::Switch")
{
    TEST_CASE("Construction creates pins A and B")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(s.pins().size() == 2);
        CHECK(s.pin("A").descriptor().name == std::string_view("A"));
        CHECK(s.pin("B").descriptor().name == std::string_view("B"));
    }

    TEST_CASE("Pins are Analog Bidirectional")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(s.pin("A").descriptor().kind == Pin::Kind::Analog);
        CHECK(s.pin("A").descriptor().direction == Pin::Direction::Bidirectional);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(&s.pin(Switch::Terminal::A) == &s.pin("A"));
        CHECK(&s.pin(Switch::Terminal::B) == &s.pin("B"));
    }

    TEST_CASE("Initial state matches descriptor")
    {
        Switch open{ Switch::Descriptor{ .name = "S1", .initialState = Switch::State::Open } };
        CHECK(open.state() == Switch::State::Open);

        Switch closed{ Switch::Descriptor{ .name = "S2", .initialState = Switch::State::Closed } };
        CHECK(closed.state() == Switch::State::Closed);
    }

    TEST_CASE("close() and open() update state")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        s.close();
        CHECK(s.state() == Switch::State::Closed);

        s.open();
        CHECK(s.state() == Switch::State::Open);
    }

    TEST_CASE("R() reflects contact state")
    {
        Switch s{ Switch::Descriptor{ .name = "S1", .initialState = Switch::State::Closed } };

        CHECK(s.R() == 0.0f);

        s.open();
        CHECK(s.R() > 0.0f);
    }

    TEST_CASE("L() and C() default to zero")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(s.L() == 0.0f);
        CHECK(s.C() == 0.0f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(s.name() == std::string_view("S1"));
    }
}
