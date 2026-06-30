#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::discrete;

TEST_SUITE("discrete::Diode")
{
    TEST_CASE("Construction creates pins A (Anode) and K (Cathode)")
    {
        Diode d{ Diode::Descriptor{ .name = "D1" } };

        CHECK(d.pins().size() == 2);
        CHECK(d.pin("A").descriptor().name == std::string_view("A"));
        CHECK(d.pin("K").descriptor().name == std::string_view("K"));
    }

    TEST_CASE("Pins are Bidirectional")
    {
        Diode d{ Diode::Descriptor{ .name = "D1" } };

        CHECK(d.pin("A").descriptor().direction == Pin::Direction::Bidirectional);
        CHECK(d.pin("K").descriptor().direction == Pin::Direction::Bidirectional);
    }

    TEST_CASE("Pins are Analog")
    {
        Diode d{ Diode::Descriptor{ .name = "D1" } };

        CHECK(d.pin("A").descriptor().kind == Pin::Kind::Analog);
        CHECK(d.pin("K").descriptor().kind == Pin::Kind::Analog);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        Diode d{ Diode::Descriptor{ .name = "D1" } };

        CHECK(&d.pin(Diode::Terminal::A) == &d.pin("A"));
        CHECK(&d.pin(Diode::Terminal::K) == &d.pin("K"));
    }

    TEST_CASE("Default descriptor values (ideal diode)")
    {
        Diode d{ Diode::Descriptor{ .name = "D1" } };

        CHECK(d.forwardVoltage() == 0.0f);
        CHECK(d.saturationCurrent() == 0.0f);
        CHECK(d.emissionCoefficient() == 1.0f);
    }

    TEST_CASE("Silicon diode threshold model")
    {
        Diode d(Diode::Descriptor{ .name = "D1", .forwardVoltage = 0.7f });

        CHECK(d.forwardVoltage() == 0.7f);
        CHECK(d.saturationCurrent() == 0.0f);
    }

    TEST_CASE("Shockley model parameters")
    {
        Diode d{ Diode::Descriptor{
            .name = "D1", .forwardVoltage = 0.7f, .saturationCurrent = 1e-12f, .emissionCoefficient = 1.5f } };

        CHECK(d.forwardVoltage() == 0.7f);
        CHECK(d.saturationCurrent() == 1e-12f);
        CHECK(d.emissionCoefficient() == 1.5f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        Diode d{ Diode::Descriptor{ .name = "D1" } };

        CHECK(d.name() == std::string_view("D1"));
    }
}
