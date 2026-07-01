#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;
using namespace nfx::silicon::discrete;

TEST_SUITE("discrete::LED")
{
    TEST_CASE("Construction creates pins A (Anode) and K (Cathode)")
    {
        LED led{ LED::Descriptor{ .name = "LED1" } };

        CHECK(led.pins().size() == 2);
        CHECK(led.pin("A").descriptor().name == std::string_view("A"));
        CHECK(led.pin("K").descriptor().name == std::string_view("K"));
    }

    TEST_CASE("Pins are Analog Bidirectional")
    {
        LED led{ LED::Descriptor{ .name = "LED1" } };

        CHECK(led.pin("A").descriptor().kind == Pin::Kind::Analog);
        CHECK(led.pin("A").descriptor().direction == Pin::Direction::Bidirectional);
        CHECK(led.pin("K").descriptor().direction == Pin::Direction::Bidirectional);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        LED led{ LED::Descriptor{ .name = "LED1" } };

        CHECK(&led.pin(LED::Terminal::Anode) == &led.pin("A"));
        CHECK(&led.pin(LED::Terminal::Cathode) == &led.pin("K"));
    }

    TEST_CASE("Color presets resolve to expected forward voltages")
    {
        CHECK(LED{ LED::Descriptor{ .name = "L", .color = LED::Color::Red } }.forwardVoltage() == 1.8f);
        CHECK(LED{ LED::Descriptor{ .name = "L", .color = LED::Color::Green } }.forwardVoltage() == 2.1f);
        CHECK(LED{ LED::Descriptor{ .name = "L", .color = LED::Color::Blue } }.forwardVoltage() == 3.0f);
        CHECK(LED{ LED::Descriptor{ .name = "L", .color = LED::Color::Yellow } }.forwardVoltage() == 2.0f);
        CHECK(LED{ LED::Descriptor{ .name = "L", .color = LED::Color::White } }.forwardVoltage() == 3.2f);
    }

    TEST_CASE("Color::Custom uses descriptor forwardVoltage directly")
    {
        LED led{ LED::Descriptor{ .name = "LED1", .color = LED::Color::Custom, .forwardVoltage = 2.5f } };

        CHECK(led.forwardVoltage() == 2.5f);
    }

    TEST_CASE("Default descriptor values")
    {
        LED led{ LED::Descriptor{ .name = "LED1" } };

        CHECK(led.saturationCurrent() == 0.0f);
        CHECK(led.emissionCoefficient() == 1.0f);
    }

    TEST_CASE("Shockley model parameters")
    {
        LED led{ LED::Descriptor{ .name = "LED1",
                                  .color = LED::Color::Custom,
                                  .forwardVoltage = 2.0f,
                                  .saturationCurrent = 1e-12f,
                                  .emissionCoefficient = 1.5f } };

        CHECK(led.saturationCurrent() == 1e-12f);
        CHECK(led.emissionCoefficient() == 1.5f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        LED led{ LED::Descriptor{ .name = "LED1" } };

        CHECK(led.name() == std::string_view("LED1"));
    }
}
