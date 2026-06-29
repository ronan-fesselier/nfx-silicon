#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;

TEST_SUITE("Signal::Wire")
{
    TEST_CASE("PushPull: output drives input through wire")
    {
        Pin out{ Pin::Descriptor{ .name = "OUT", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin in{ Pin::Descriptor{ .name = "IN", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Wire wire{ Wire::Descriptor{
            .name = "W1", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };

        wire.attach(out);
        wire.attach(in);

        out.drive<Level>(Level::High);
        CHECK(wire.read<Level>() == Level::High);
        CHECK(in.read<Level>() == Level::High);

        out.drive<Level>(Level::Low);
        CHECK(wire.read<Level>() == Level::Low);
        CHECK(in.read<Level>() == Level::Low);
    }

    TEST_CASE("PushPull: release sets wire to HighZ")
    {
        Pin out{ Pin::Descriptor{ .name = "OUT", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin in{ Pin::Descriptor{ .name = "IN", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Wire wire{ Wire::Descriptor{
            .name = "W1", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };

        wire.attach(out);
        wire.attach(in);

        out.drive<Level>(Level::High);
        out.release();
        CHECK(wire.read<Level>() == Level::HighZ);
    }

    TEST_CASE("PushPull: multiple receivers all get driven")
    {
        Pin out{ Pin::Descriptor{ .name = "OUT", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin in1{ Pin::Descriptor{ .name = "IN1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Pin in2{ Pin::Descriptor{ .name = "IN2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Wire wire{ Wire::Descriptor{
            .name = "W1", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };

        wire.attach(out);
        wire.attach(in1);
        wire.attach(in2);

        out.drive<Level>(Level::High);
        CHECK(in1.read<Level>() == Level::High);
        CHECK(in2.read<Level>() == Level::High);
    }

    TEST_CASE("WiredAnd: any Low pulls wire Low")
    {
        Pin od1{ Pin::Descriptor{ .name = "OD1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::OpenDrain } };
        Pin od2{ Pin::Descriptor{ .name = "OD2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::OpenDrain } };
        Pin in{ Pin::Descriptor{ .name = "IN", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Wire wire{ Wire::Descriptor{
            .name = "W1", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };

        wire.attach(od1);
        wire.attach(od2);
        wire.attach(in);

        od1.drive<Level>(Level::High);
        od2.drive<Level>(Level::High);
        CHECK(wire.read<Level>() == Level::High);

        od1.drive<Level>(Level::Low);
        CHECK(wire.read<Level>() == Level::Low);
    }

    TEST_CASE("WiredAnd: all HighZ gives HighZ")
    {
        Pin od1{ Pin::Descriptor{ .name = "OD1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::OpenDrain } };
        Pin od2{ Pin::Descriptor{ .name = "OD2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::OpenDrain } };
        Wire wire{ Wire::Descriptor{
            .name = "W1", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredAnd } };

        wire.attach(od1);
        wire.attach(od2);

        CHECK(wire.read<Level>() == Level::HighZ);
    }

    TEST_CASE("WiredOr: any High pulls wire High")
    {
        Pin out1{ Pin::Descriptor{ .name = "OUT1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin out2{ Pin::Descriptor{ .name = "OUT2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin in{ Pin::Descriptor{ .name = "IN", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Input } };
        Wire wire{ Wire::Descriptor{
            .name = "W1", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredOr } };

        wire.attach(out1);
        wire.attach(out2);
        wire.attach(in);

        out1.drive<Level>(Level::Low);
        out2.drive<Level>(Level::Low);
        CHECK(wire.read<Level>() == Level::Low);

        out1.drive<Level>(Level::High);
        CHECK(wire.read<Level>() == Level::High);
    }

    TEST_CASE("WiredOr: all HighZ gives HighZ")
    {
        Pin out1{ Pin::Descriptor{ .name = "OUT1", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Pin out2{ Pin::Descriptor{ .name = "OUT2", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wire{ Wire::Descriptor{
            .name = "W1", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::WiredOr } };

        wire.attach(out1);
        wire.attach(out2);

        CHECK(wire.read<Level>() == Level::HighZ);
    }

    TEST_CASE("Analog: voltage propagates to receiver")
    {
        Pin out{ Pin::Descriptor{ .name = "AOUT", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };
        Pin in{ Pin::Descriptor{ .name = "AIN", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Input } };
        Wire wire{ Wire::Descriptor{ .name = "W1", .kind = Pin::Kind::Analog } };

        wire.attach(out);
        wire.attach(in);

        out.drive<float>(3.3f);
        REQUIRE(wire.read<float>().has_value());
        CHECK(wire.read<float>().value() == 3.3f);
        REQUIRE(in.read<float>().has_value());
        CHECK(in.read<float>().value() == 3.3f);
    }

    TEST_CASE("Analog: release sets wire and receiver to floating")
    {
        Pin out{ Pin::Descriptor{ .name = "AOUT", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };
        Pin in{ Pin::Descriptor{ .name = "AIN", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Input } };
        Wire wire{ Wire::Descriptor{ .name = "W1", .kind = Pin::Kind::Analog } };

        wire.attach(out);
        wire.attach(in);

        out.drive<float>(5.0f);
        out.release();
        CHECK(wire.read<float>() == std::nullopt);
        CHECK(in.read<float>() == std::nullopt);
    }

    TEST_CASE("Bidirectional pin acts as both driver and receiver")
    {
        Pin bidir{ Pin::Descriptor{
            .name = "BD", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Bidirectional } };
        Pin out{ Pin::Descriptor{ .name = "OUT", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        Wire wire{ Wire::Descriptor{
            .name = "W1", .kind = Pin::Kind::Digital, .resolution = Wire::Resolution::PushPull } };

        wire.attach(bidir);
        wire.attach(out);

        out.drive<Level>(Level::High);
        CHECK(bidir.read<Level>() == Level::High);
    }
}
