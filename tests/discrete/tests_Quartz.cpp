#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::discrete;

TEST_SUITE("discrete::Quartz")
{
    TEST_CASE("Construction creates 2 pins")
    {
        Quartz y{ Quartz::Descriptor{ .name = "Y1", .frequencyHz = 1'000'000.0f } };

        CHECK(y.pins().size() == 2);
    }

    TEST_CASE("Pin names are X1 and X2")
    {
        Quartz y{ Quartz::Descriptor{ .name = "Y1", .frequencyHz = 1'000'000.0f } };

        CHECK(y.pin("X1").descriptor().name == std::string_view("X1"));
        CHECK(y.pin("X2").descriptor().name == std::string_view("X2"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        Quartz y{ Quartz::Descriptor{ .name = "Y1", .frequencyHz = 1'000'000.0f } };

        CHECK(&y.pin(Quartz::Terminal::X1) == &y.pin("X1"));
        CHECK(&y.pin(Quartz::Terminal::X2) == &y.pin("X2"));
    }

    TEST_CASE("Pins are Analog")
    {
        Quartz y{ Quartz::Descriptor{ .name = "Y1", .frequencyHz = 1'000'000.0f } };

        CHECK(y.pin("X1").descriptor().kind == Pin::Kind::Analog);
        CHECK(y.pin("X2").descriptor().kind == Pin::Kind::Analog);
    }

    TEST_CASE("Pins are Bidirectional")
    {
        Quartz y{ Quartz::Descriptor{ .name = "Y1", .frequencyHz = 1'000'000.0f } };

        CHECK(y.pin("X1").descriptor().direction == Pin::Direction::Bidirectional);
        CHECK(y.pin("X2").descriptor().direction == Pin::Direction::Bidirectional);
    }

    TEST_CASE("frequencyHz returns the configured frequency")
    {
        Quartz y{ Quartz::Descriptor{ .name = "Y1", .frequencyHz = 1'760'000.0f } };

        CHECK(y.frequencyHz() == 1'760'000.0f);
    }

    TEST_CASE("descriptor returns the configured descriptor")
    {
        Quartz y{ Quartz::Descriptor{ .name = "Y1", .frequencyHz = 3'579'545.0f } };

        CHECK(y.descriptor().name == std::string_view("Y1"));
        CHECK(y.descriptor().frequencyHz == 3'579'545.0f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        Quartz y{ Quartz::Descriptor{ .name = "Y1", .frequencyHz = 1'000'000.0f } };

        CHECK(y.name() == std::string_view("Y1"));
    }
}
