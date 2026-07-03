#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::gate;

TEST_SUITE("gate::Not")
{
    TEST_CASE("Construction creates pins A and Y")
    {
        Not g{ Not::Descriptor{ .name = "U1" } };

        CHECK(g.pins().size() == 2);
        CHECK(g.pin("A").descriptor().name == std::string_view("A"));
        CHECK(g.pin("Y").descriptor().name == std::string_view("Y"));
    }

    TEST_CASE("Pin A is Digital Input, Y is Digital Output")
    {
        Not g{ Not::Descriptor{ .name = "U1" } };

        CHECK(g.pin("A").descriptor().kind == Pin::Kind::Digital);
        CHECK(g.pin("A").descriptor().direction == Pin::Direction::Input);
        CHECK(g.pin("Y").descriptor().kind == Pin::Kind::Digital);
        CHECK(g.pin("Y").descriptor().direction == Pin::Direction::Output);
    }

    TEST_CASE("Pin enum maps to correct pins")
    {
        Not g{ Not::Descriptor{ .name = "U1" } };

        CHECK(&g.pin(Not::Terminal::A) == &g.pin("A"));
        CHECK(&g.pin(Not::Terminal::Y) == &g.pin("Y"));
    }

    TEST_CASE("Truth table: Low -> High")
    {
        Not g{ Not::Descriptor{ .name = "U1" } };
        g.pin("A").drive<Level>(Level::Low);

        CHECK(g.pin("Y").read<Level>() == Level::High);
    }

    TEST_CASE("Truth table: High -> Low")
    {
        Not g{ Not::Descriptor{ .name = "U1" } };
        g.pin("A").drive<Level>(Level::High);

        CHECK(g.pin("Y").read<Level>() == Level::Low);
    }

    TEST_CASE("Truth table: HighZ -> HighZ")
    {
        Not g{ Not::Descriptor{ .name = "U1" } };
        g.pin("A").drive<Level>(Level::Low);
        g.pin("A").release();

        CHECK(g.pin("Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        Not g{ Not::Descriptor{ .name = "U1" } };

        CHECK(g.name() == std::string_view("U1"));
    }
}

TEST_SUITE("gate::And")
{
    TEST_CASE("Construction creates pins A, B and Y")
    {
        And g{ And::Descriptor{ .name = "U1" } };

        CHECK(g.pins().size() == 3);
        CHECK(g.pin("A").descriptor().name == std::string_view("A"));
        CHECK(g.pin("B").descriptor().name == std::string_view("B"));
        CHECK(g.pin("Y").descriptor().name == std::string_view("Y"));
    }

    TEST_CASE("Pins A and B are Digital Input, Y is Digital Output")
    {
        And g{ And::Descriptor{ .name = "U1" } };

        CHECK(g.pin("A").descriptor().kind == Pin::Kind::Digital);
        CHECK(g.pin("A").descriptor().direction == Pin::Direction::Input);
        CHECK(g.pin("B").descriptor().kind == Pin::Kind::Digital);
        CHECK(g.pin("B").descriptor().direction == Pin::Direction::Input);
        CHECK(g.pin("Y").descriptor().kind == Pin::Kind::Digital);
        CHECK(g.pin("Y").descriptor().direction == Pin::Direction::Output);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        And g{ And::Descriptor{ .name = "U1" } };

        CHECK(&g.pin(And::Terminal::A) == &g.pin("A"));
        CHECK(&g.pin(And::Terminal::B) == &g.pin("B"));
        CHECK(&g.pin(And::Terminal::Y) == &g.pin("Y"));
    }

    TEST_CASE("Truth table: Low & Low -> Low")
    {
        And g{ And::Descriptor{ .name = "U1" } };
        g.pin("A").drive<Level>(Level::Low);
        g.pin("B").drive<Level>(Level::Low);

        CHECK(g.pin("Y").read<Level>() == Level::Low);
    }

    TEST_CASE("Truth table: Low & High -> Low")
    {
        And g{ And::Descriptor{ .name = "U1" } };
        g.pin("A").drive<Level>(Level::Low);
        g.pin("B").drive<Level>(Level::High);

        CHECK(g.pin("Y").read<Level>() == Level::Low);
    }

    TEST_CASE("Truth table: High & Low -> Low")
    {
        And g{ And::Descriptor{ .name = "U1" } };
        g.pin("A").drive<Level>(Level::High);
        g.pin("B").drive<Level>(Level::Low);

        CHECK(g.pin("Y").read<Level>() == Level::Low);
    }

    TEST_CASE("Truth table: High & High -> High")
    {
        And g{ And::Descriptor{ .name = "U1" } };
        g.pin("A").drive<Level>(Level::High);
        g.pin("B").drive<Level>(Level::High);

        CHECK(g.pin("Y").read<Level>() == Level::High);
    }

    TEST_CASE("Truth table: HighZ on A -> HighZ")
    {
        And g{ And::Descriptor{ .name = "U1" } };
        g.pin("A").drive<Level>(Level::High);
        g.pin("B").drive<Level>(Level::High);
        g.pin("A").release();

        CHECK(g.pin("Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Truth table: HighZ on B -> HighZ")
    {
        And g{ And::Descriptor{ .name = "U1" } };
        g.pin("A").drive<Level>(Level::High);
        g.pin("B").drive<Level>(Level::High);
        g.pin("B").release();

        CHECK(g.pin("Y").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name is stored correctly")
    {
        And g{ And::Descriptor{ .name = "U1" } };

        CHECK(g.name() == std::string_view("U1"));
    }
}
