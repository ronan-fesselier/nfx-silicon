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
