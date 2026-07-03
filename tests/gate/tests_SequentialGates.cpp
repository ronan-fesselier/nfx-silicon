#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::gate;

TEST_SUITE("gate::SRLatch")
{
    TEST_CASE("Construction creates pins S, R, Q and NQ")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };

        CHECK(g.pins().size() == 4);
        CHECK(g.pin("S").descriptor().name == std::string_view("S"));
        CHECK(g.pin("R").descriptor().name == std::string_view("R"));
        CHECK(g.pin("Q").descriptor().name == std::string_view("Q"));
        CHECK(g.pin("NQ").descriptor().name == std::string_view("NQ"));
    }

    TEST_CASE("Pin kinds and directions are correct")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };

        CHECK(g.pin("S").descriptor().kind == Pin::Kind::Digital);
        CHECK(g.pin("S").descriptor().direction == Pin::Direction::Input);
        CHECK(g.pin("R").descriptor().kind == Pin::Kind::Digital);
        CHECK(g.pin("R").descriptor().direction == Pin::Direction::Input);
        CHECK(g.pin("Q").descriptor().kind == Pin::Kind::Digital);
        CHECK(g.pin("Q").descriptor().direction == Pin::Direction::Output);
        CHECK(g.pin("NQ").descriptor().kind == Pin::Kind::Digital);
        CHECK(g.pin("NQ").descriptor().direction == Pin::Direction::Output);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };

        CHECK(&g.pin(SRLatch::Terminal::S) == &g.pin("S"));
        CHECK(&g.pin(SRLatch::Terminal::R) == &g.pin("R"));
        CHECK(&g.pin(SRLatch::Terminal::Q) == &g.pin("Q"));
        CHECK(&g.pin(SRLatch::Terminal::NQ) == &g.pin("NQ"));
    }

    TEST_CASE("Truth table: S=High R=Low -> Q=High NQ=Low")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };
        g.pin("S").drive<Level>(Level::High);
        g.pin("R").drive<Level>(Level::Low);

        CHECK(g.pin("Q").read<Level>() == Level::High);
        CHECK(g.pin("NQ").read<Level>() == Level::Low);
    }

    TEST_CASE("Truth table: S=Low R=High -> Q=Low NQ=High")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };
        g.pin("S").drive<Level>(Level::Low);
        g.pin("R").drive<Level>(Level::High);

        CHECK(g.pin("Q").read<Level>() == Level::Low);
        CHECK(g.pin("NQ").read<Level>() == Level::High);
    }

    TEST_CASE("Truth table: S=High R=High -> Q=HighZ NQ=HighZ (forbidden)")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };
        g.pin("S").drive<Level>(Level::High);
        g.pin("R").drive<Level>(Level::High);

        CHECK(g.pin("Q").read<Level>() == Level::HighZ);
        CHECK(g.pin("NQ").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Truth table: S=Low R=Low -> hold state")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };

        // Set first
        g.pin("S").drive<Level>(Level::High);
        g.pin("R").drive<Level>(Level::Low);
        CHECK(g.pin("Q").read<Level>() == Level::High);

        // Then hold
        g.pin("S").drive<Level>(Level::Low);
        CHECK(g.pin("Q").read<Level>() == Level::High);
        CHECK(g.pin("NQ").read<Level>() == Level::Low);
    }

    TEST_CASE("HighZ on S holds state")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };

        g.pin("S").drive<Level>(Level::High);
        g.pin("R").drive<Level>(Level::Low);
        CHECK(g.pin("Q").read<Level>() == Level::High);

        g.pin("S").release();
        CHECK(g.pin("Q").read<Level>() == Level::High);
        CHECK(g.pin("NQ").read<Level>() == Level::Low);
    }

    TEST_CASE("HighZ on R holds state")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };

        g.pin("S").drive<Level>(Level::Low);
        g.pin("R").drive<Level>(Level::High);
        CHECK(g.pin("Q").read<Level>() == Level::Low);

        g.pin("R").release();
        CHECK(g.pin("Q").read<Level>() == Level::Low);
        CHECK(g.pin("NQ").read<Level>() == Level::High);
    }

    TEST_CASE("reset() drives Q=Low NQ=High")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };

        g.pin("S").drive<Level>(Level::High);
        g.pin("R").drive<Level>(Level::Low);
        CHECK(g.pin("Q").read<Level>() == Level::High);

        g.reset();

        CHECK(g.pin("Q").read<Level>() == Level::Low);
        CHECK(g.pin("NQ").read<Level>() == Level::High);
    }

    TEST_CASE("Component name is stored correctly")
    {
        SRLatch g{ SRLatch::Descriptor{ .name = "U1" } };

        CHECK(g.name() == std::string_view("U1"));
    }
}
