#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::discrete;

TEST_SUITE("discrete::Resistor")
{
    TEST_CASE("Construction creates pins A and B")
    {
        Resistor r{ Resistor::Descriptor{ .name = "R1", .ohms = 220.0f } };

        CHECK(r.pins().size() == 2);
        CHECK(r.pin("A").descriptor().name == std::string_view("A"));
        CHECK(r.pin("B").descriptor().name == std::string_view("B"));
    }

    TEST_CASE("Pins are Analog Bidirectional")
    {
        Resistor r{ Resistor::Descriptor{ .name = "R1", .ohms = 220.0f } };

        CHECK(r.pin("A").descriptor().kind == Pin::Kind::Analog);
        CHECK(r.pin("A").descriptor().direction == Pin::Direction::Bidirectional);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        Resistor r{ Resistor::Descriptor{ .name = "R1", .ohms = 220.0f } };

        CHECK(&r.pin(Resistor::Terminal::A) == &r.pin("A"));
        CHECK(&r.pin(Resistor::Terminal::B) == &r.pin("B"));
    }

    TEST_CASE("R() returns configured value, L() and C() default to zero")
    {
        Resistor r{ Resistor::Descriptor{ .name = "R1", .ohms = 220.0f } };

        CHECK(r.R() == 220.0f);
        CHECK(r.L() == 0.0f);
        CHECK(r.C() == 0.0f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        Resistor r{ Resistor::Descriptor{ .name = "R1", .ohms = 220.0f } };

        CHECK(r.name() == std::string_view("R1"));
    }
}

TEST_SUITE("discrete::Capacitor")
{
    TEST_CASE("Construction creates pins A and B")
    {
        Capacitor c{ Capacitor::Descriptor{ .name = "C1", .farads = 100e-9f } };

        CHECK(c.pins().size() == 2);
        CHECK(c.pin("A").descriptor().name == std::string_view("A"));
        CHECK(c.pin("B").descriptor().name == std::string_view("B"));
    }

    TEST_CASE("Pins are Analog Bidirectional")
    {
        Capacitor c{ Capacitor::Descriptor{ .name = "C1", .farads = 100e-9f } };

        CHECK(c.pin("A").descriptor().kind == Pin::Kind::Analog);
        CHECK(c.pin("A").descriptor().direction == Pin::Direction::Bidirectional);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        Capacitor c{ Capacitor::Descriptor{ .name = "C1", .farads = 100e-9f } };

        CHECK(&c.pin(Capacitor::Terminal::A) == &c.pin("A"));
        CHECK(&c.pin(Capacitor::Terminal::B) == &c.pin("B"));
    }

    TEST_CASE("C() returns configured value, R() and L() default to zero")
    {
        Capacitor c{ Capacitor::Descriptor{ .name = "C1", .farads = 100e-9f } };

        CHECK(c.C() == 100e-9f);
        CHECK(c.R() == 0.0f);
        CHECK(c.L() == 0.0f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        Capacitor c{ Capacitor::Descriptor{ .name = "C1", .farads = 100e-9f } };

        CHECK(c.name() == std::string_view("C1"));
    }
}

TEST_SUITE("discrete::Inductor")
{
    TEST_CASE("Construction creates pins A and B")
    {
        Inductor l{ Inductor::Descriptor{ .name = "L1", .henries = 10e-3f } };

        CHECK(l.pins().size() == 2);
        CHECK(l.pin("A").descriptor().name == std::string_view("A"));
        CHECK(l.pin("B").descriptor().name == std::string_view("B"));
    }

    TEST_CASE("Pins are Analog Bidirectional")
    {
        Inductor l{ Inductor::Descriptor{ .name = "L1", .henries = 10e-3f } };

        CHECK(l.pin("A").descriptor().kind == Pin::Kind::Analog);
        CHECK(l.pin("A").descriptor().direction == Pin::Direction::Bidirectional);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        Inductor l{ Inductor::Descriptor{ .name = "L1", .henries = 10e-3f } };

        CHECK(&l.pin(Inductor::Terminal::A) == &l.pin("A"));
        CHECK(&l.pin(Inductor::Terminal::B) == &l.pin("B"));
    }

    TEST_CASE("L() returns configured value, R() and C() default to zero")
    {
        Inductor l{ Inductor::Descriptor{ .name = "L1", .henries = 10e-3f } };

        CHECK(l.L() == 10e-3f);
        CHECK(l.R() == 0.0f);
        CHECK(l.C() == 0.0f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        Inductor l{ Inductor::Descriptor{ .name = "L1", .henries = 10e-3f } };

        CHECK(l.name() == std::string_view("L1"));
    }
}
