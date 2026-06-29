#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;

TEST_SUITE("Signal::Bus")
{
    TEST_CASE("Construction and width")
    {
        Pin out0{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin out1{ Pin::Descriptor{ .name = "D1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire w0{ Wire::Descriptor{ .name = "W0", .kind = Pin::Kind::Digital } };
        Wire w1{ Wire::Descriptor{ .name = "W1", .kind = Pin::Kind::Digital } };

        w0.attach(out0);
        w1.attach(out1);

        Wire* wires[] = { &w0, &w1 };
        Bus bus(Bus::Descriptor{ .name = "DATA", .kind = Pin::Kind::Digital, .wires = wires });

        CHECK(bus.width() == 2);
        CHECK(bus.descriptor().name == std::string_view("DATA"));
    }

    TEST_CASE("Wire access by index")
    {
        Pin out0{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin out1{ Pin::Descriptor{ .name = "D1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire w0{ Wire::Descriptor{ .name = "W0", .kind = Pin::Kind::Digital } };
        Wire w1{ Wire::Descriptor{ .name = "W1", .kind = Pin::Kind::Digital } };

        w0.attach(out0);
        w1.attach(out1);

        Wire* wires[] = { &w0, &w1 };
        Bus bus(Bus::Descriptor{ .name = "DATA", .kind = Pin::Kind::Digital, .wires = wires });

        CHECK(&bus.wire(0) == &w0);
        CHECK(&bus.wire(1) == &w1);
    }

    TEST_CASE("Digital read returns levels from all wires")
    {
        Pin out0{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin out1{ Pin::Descriptor{ .name = "D1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin in0(Pin::Descriptor{ .name = "IN0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input });
        Pin in1(Pin::Descriptor{ .name = "IN1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input });
        Wire w0{ Wire::Descriptor{ .name = "W0", .kind = Pin::Kind::Digital } };
        Wire w1{ Wire::Descriptor{ .name = "W1", .kind = Pin::Kind::Digital } };

        w0.attach(out0);
        w0.attach(in0);
        w1.attach(out1);
        w1.attach(in1);

        Wire* wires[] = { &w0, &w1 };
        Bus bus(Bus::Descriptor{ .name = "DATA", .kind = Pin::Kind::Digital, .wires = wires });

        out0.drive<Level>(Level::High);
        out1.drive<Level>(Level::Low);

        Level levels[2];
        bus.read<Level>(levels);
        CHECK(levels[0] == Level::High);
        CHECK(levels[1] == Level::Low);
    }

    TEST_CASE("Analog read returns voltages from all wires")
    {
        Pin out0{ Pin::Descriptor{ .name = "A0", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };
        Pin out1{ Pin::Descriptor{ .name = "A1", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };
        Wire w0{ Wire::Descriptor{ .name = "W0", .kind = Pin::Kind::Analog } };
        Wire w1{ Wire::Descriptor{ .name = "W1", .kind = Pin::Kind::Analog } };

        w0.attach(out0);
        w1.attach(out1);

        Wire* wires[] = { &w0, &w1 };
        Bus bus(Bus::Descriptor{ .name = "ANALOG", .kind = Pin::Kind::Analog, .wires = wires });

        out0.drive<float>(3.3f);
        out1.drive<float>(1.8f);

        Voltage voltages[2];
        bus.read<float>(voltages);
        REQUIRE(voltages[0].has_value());
        CHECK(voltages[0].value() == 3.3f);
        REQUIRE(voltages[1].has_value());
        CHECK(voltages[1].value() == 1.8f);
    }

    TEST_CASE("Undriven bus reads HighZ / nullopt")
    {
        Pin out0(Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output });
        Wire w0(Wire::Descriptor{ .name = "W0", .kind = Pin::Kind::Digital });

        w0.attach(out0);

        Wire* wires[] = { &w0 };
        Bus bus(Bus::Descriptor{ .name = "DATA", .kind = Pin::Kind::Digital, .wires = wires });

        Level levels[1];
        bus.read<Level>(levels);
        CHECK(levels[0] == Level::HighZ);
    }
}
