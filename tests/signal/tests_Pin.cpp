#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::signal;

TEST_SUITE("Signal::Pin")
{
    TEST_CASE("Digital pin initial state is HighZ")
    {
        Pin pin{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

        CHECK(pin.read<Level>() == Level::HighZ);
    }

    TEST_CASE("Analog pin initial state is floating (nullopt)")
    {
        Pin pin{ Pin::Descriptor{ .name = "A0", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Input } };

        CHECK(pin.read<float>() == std::nullopt);
    }

    TEST_CASE("Digital pin drive and read")
    {
        Pin pin{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

        pin.drive<Level>(Level::High);
        CHECK(pin.read<Level>() == Level::High);

        pin.drive<Level>(Level::Low);
        CHECK(pin.read<Level>() == Level::Low);
    }

    TEST_CASE("Analog pin drive and read")
    {
        Pin pin{ Pin::Descriptor{ .name = "A0", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };

        pin.drive<float>(3.3f);
        REQUIRE(pin.read<float>().has_value());
        CHECK(pin.read<float>().value() == 3.3f);
    }

    TEST_CASE("Digital pin release sets HighZ")
    {
        Pin pin{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

        pin.drive<Level>(Level::High);
        pin.release();
        CHECK(pin.read<Level>() == Level::HighZ);
    }

    TEST_CASE("Analog pin release sets nullopt")
    {
        Pin pin{ Pin::Descriptor{ .name = "A0", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };

        pin.drive<float>(5.0f);
        pin.release();
        CHECK(pin.read<float>() == std::nullopt);
    }

    TEST_CASE("Digital pin does not notify when driven to same level")
    {
        Pin pin{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

        int callCount = 0;
        pin.connect<Level>([&callCount](Level) { callCount++; });

        pin.drive<Level>(Level::High);
        CHECK(callCount == 1);

        pin.drive<Level>(Level::High);
        CHECK(callCount == 1);
    }

    TEST_CASE("Analog pin always notifies on drive")
    {
        Pin pin{ Pin::Descriptor{ .name = "A0", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };

        int callCount = 0;
        float lastValue = 0.0f;
        pin.connect<float>([&callCount, &lastValue](Voltage v) {
            callCount++;
            if (v.has_value())
            {
                lastValue = v.value();
            }
        });

        pin.drive<float>(3.3f);
        CHECK(callCount == 1);
        CHECK(lastValue == 3.3f);

        pin.drive<float>(3.3f);
        CHECK(callCount == 2);
    }

    TEST_CASE("Digital pin release notifies observers with HighZ")
    {
        Pin pin{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

        Level lastLevel = Level::Low;
        pin.connect<Level>([&lastLevel](Level l) { lastLevel = l; });

        pin.drive<Level>(Level::High);
        pin.release();
        CHECK(lastLevel == Level::HighZ);
    }

    TEST_CASE("Analog pin release notifies observers with nullopt")
    {
        Pin pin{ Pin::Descriptor{ .name = "A0", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };

        int callCount = 0;
        Voltage lastVoltage{ 0.0f };
        pin.connect<float>([&callCount, &lastVoltage](Voltage v) {
            callCount++;
            lastVoltage = v;
        });

        pin.drive<float>(5.0f);
        CHECK(callCount == 1);
        REQUIRE(lastVoltage.has_value());
        CHECK(lastVoltage.value() == 5.0f);

        pin.release();
        CHECK(callCount == 2);
        CHECK(lastVoltage == std::nullopt);
    }

    TEST_CASE("Multiple observers are all notified")
    {
        Pin pin{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

        int count1 = 0;
        int count2 = 0;
        pin.connect<Level>([&count1](Level) { count1++; });
        pin.connect<Level>([&count2](Level) { count2++; });

        pin.drive<Level>(Level::High);
        CHECK(count1 == 1);
        CHECK(count2 == 1);
    }

    TEST_CASE("Digital connect fires immediately if pin already driven")
    {
        Pin pin{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };
        pin.drive<Level>(Level::High);

        Level received = Level::HighZ;
        pin.connect<Level>([&received](const Level l) { received = l; });

        CHECK(received == Level::High);
    }

    TEST_CASE("Digital connect does not fire if pin is HighZ")
    {
        Pin pin{ Pin::Descriptor{ .name = "D0", .kind = Pin::Kind::Digital, .direction = Pin::Direction::Output } };

        int callCount = 0;
        pin.connect<Level>([&callCount](Level) { callCount++; });

        CHECK(callCount == 0);
    }

    TEST_CASE("Analog connect fires immediately if pin already driven")
    {
        Pin pin{ Pin::Descriptor{ .name = "A0", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };
        pin.drive<float>(3.3f);

        Voltage received = std::nullopt;
        pin.connect<float>([&received](const Voltage v) { received = v; });

        CHECK(received.has_value());
        CHECK(received.value() == doctest::Approx(3.3f));
    }

    TEST_CASE("Analog connect does not fire if pin is floating")
    {
        Pin pin{ Pin::Descriptor{ .name = "A0", .kind = Pin::Kind::Analog, .direction = Pin::Direction::Output } };

        int callCount = 0;
        pin.connect<float>([&callCount](Voltage) { callCount++; });

        CHECK(callCount == 0);
    }
}
