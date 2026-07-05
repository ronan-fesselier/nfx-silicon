#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::discrete;
using namespace nfx::silicon::signal;

TEST_SUITE("discrete::Switch")
{
    TEST_CASE("Construction creates pins A and B")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(s.pins().size() == 2);
        CHECK(s.pin("A").descriptor().name == std::string_view("A"));
        CHECK(s.pin("B").descriptor().name == std::string_view("B"));
    }

    TEST_CASE("Pins are Analog Bidirectional")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(s.pin("A").descriptor().kind == Pin::Kind::Analog);
        CHECK(s.pin("A").descriptor().direction == Pin::Direction::Bidirectional);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(&s.pin(Switch::Terminal::A) == &s.pin("A"));
        CHECK(&s.pin(Switch::Terminal::B) == &s.pin("B"));
    }

    TEST_CASE("Initial state matches descriptor")
    {
        Switch open{ Switch::Descriptor{ .name = "S1", .initialState = Switch::State::Open } };
        CHECK(open.state() == Switch::State::Open);

        Switch closed{ Switch::Descriptor{ .name = "S2", .initialState = Switch::State::Closed } };
        CHECK(closed.state() == Switch::State::Closed);
    }

    TEST_CASE("close() and open() update state")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        s.close();
        CHECK(s.state() == Switch::State::Closed);

        s.open();
        CHECK(s.state() == Switch::State::Open);
    }

    TEST_CASE("R() reflects contact state")
    {
        Switch s{ Switch::Descriptor{ .name = "S1", .initialState = Switch::State::Closed } };

        CHECK(s.R() == 0.0f);

        s.open();
        CHECK(s.R() > 0.0f);
    }

    TEST_CASE("L() and C() default to zero")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(s.L() == 0.0f);
        CHECK(s.C() == 0.0f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        Switch s{ Switch::Descriptor{ .name = "S1" } };

        CHECK(s.name() == std::string_view("S1"));
    }
}

TEST_SUITE("discrete::PushButton")
{
    TEST_CASE("Construction creates pins A and B")
    {
        PushButton btn{ PushButton::Descriptor{ .name = "BTN1" } };

        CHECK(btn.pins().size() == 2);
        CHECK(btn.pin("A").descriptor().name == std::string_view("A"));
        CHECK(btn.pin("B").descriptor().name == std::string_view("B"));
    }

    TEST_CASE("Pins are Analog Bidirectional")
    {
        PushButton btn{ PushButton::Descriptor{ .name = "BTN1" } };

        CHECK(btn.pin("A").descriptor().kind == Pin::Kind::Analog);
        CHECK(btn.pin("A").descriptor().direction == Pin::Direction::Bidirectional);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        PushButton btn{ PushButton::Descriptor{ .name = "BTN1" } };

        CHECK(&btn.pin(PushButton::Terminal::A) == &btn.pin("A"));
        CHECK(&btn.pin(PushButton::Terminal::B) == &btn.pin("B"));
    }

    TEST_CASE("NormallyOpen: R() large when released, 0.0f when pressed")
    {
        PushButton btn{ PushButton::Descriptor{ .name = "BTN1",
                                                .contactType = PushButton::ContactType::NormallyOpen } };

        CHECK(btn.R() > 0.0f);

        btn.press();
        CHECK(btn.R() == 0.0f);

        btn.release();
        CHECK(btn.R() > 0.0f);
    }

    TEST_CASE("NormallyClosed: R() 0.0f when released, large when pressed")
    {
        PushButton btn{ PushButton::Descriptor{ .name = "BTN1",
                                                .contactType = PushButton::ContactType::NormallyClosed } };

        CHECK(btn.R() == 0.0f);

        btn.press();
        CHECK(btn.R() > 0.0f);

        btn.release();
        CHECK(btn.R() == 0.0f);
    }

    TEST_CASE("isPressed() reflects current state")
    {
        PushButton btn{ PushButton::Descriptor{ .name = "BTN1" } };

        CHECK_FALSE(btn.isPressed());

        btn.press();
        CHECK(btn.isPressed());

        btn.release();
        CHECK_FALSE(btn.isPressed());
    }

    TEST_CASE("L() and C() default to zero")
    {
        PushButton btn{ PushButton::Descriptor{ .name = "BTN1" } };

        CHECK(btn.L() == 0.0f);
        CHECK(btn.C() == 0.0f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        PushButton btn{ PushButton::Descriptor{ .name = "BTN1" } };

        CHECK(btn.name() == std::string_view("BTN1"));
    }
}
