#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::chip::cdp1xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("chip::cdp1xxx::CDP1852")
{
    TEST_CASE("Construction creates 24 pins")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        CHECK(u.pins().size() == 24);
    }

    TEST_CASE("Pin names match DIP-24 datasheet")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        CHECK(u.pin("nCS1_CS1").descriptor().name == std::string_view("nCS1_CS1"));
        CHECK(u.pin("MODE").descriptor().name == std::string_view("MODE"));
        CHECK(u.pin("DI0").descriptor().name == std::string_view("DI0"));
        CHECK(u.pin("DO0").descriptor().name == std::string_view("DO0"));
        CHECK(u.pin("DI1").descriptor().name == std::string_view("DI1"));
        CHECK(u.pin("DO1").descriptor().name == std::string_view("DO1"));
        CHECK(u.pin("DI2").descriptor().name == std::string_view("DI2"));
        CHECK(u.pin("DO2").descriptor().name == std::string_view("DO2"));
        CHECK(u.pin("DI3").descriptor().name == std::string_view("DI3"));
        CHECK(u.pin("DO3").descriptor().name == std::string_view("DO3"));
        CHECK(u.pin("CLOCK").descriptor().name == std::string_view("CLOCK"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("CS2").descriptor().name == std::string_view("CS2"));
        CHECK(u.pin("CLEAR").descriptor().name == std::string_view("CLEAR"));
        CHECK(u.pin("DO4").descriptor().name == std::string_view("DO4"));
        CHECK(u.pin("DI4").descriptor().name == std::string_view("DI4"));
        CHECK(u.pin("DO5").descriptor().name == std::string_view("DO5"));
        CHECK(u.pin("DI5").descriptor().name == std::string_view("DI5"));
        CHECK(u.pin("DO6").descriptor().name == std::string_view("DO6"));
        CHECK(u.pin("DI6").descriptor().name == std::string_view("DI6"));
        CHECK(u.pin("DO7").descriptor().name == std::string_view("DO7"));
        CHECK(u.pin("DI7").descriptor().name == std::string_view("DI7"));
        CHECK(u.pin("nSR_SR").descriptor().name == std::string_view("nSR_SR"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        CHECK(&u.pin(CDP1852::Terminal::nCS1_CS1) == &u.pin("nCS1_CS1"));
        CHECK(&u.pin(CDP1852::Terminal::MODE) == &u.pin("MODE"));
        CHECK(&u.pin(CDP1852::Terminal::DI0) == &u.pin("DI0"));
        CHECK(&u.pin(CDP1852::Terminal::DO0) == &u.pin("DO0"));
        CHECK(&u.pin(CDP1852::Terminal::DI1) == &u.pin("DI1"));
        CHECK(&u.pin(CDP1852::Terminal::DO1) == &u.pin("DO1"));
        CHECK(&u.pin(CDP1852::Terminal::DI2) == &u.pin("DI2"));
        CHECK(&u.pin(CDP1852::Terminal::DO2) == &u.pin("DO2"));
        CHECK(&u.pin(CDP1852::Terminal::DI3) == &u.pin("DI3"));
        CHECK(&u.pin(CDP1852::Terminal::DO3) == &u.pin("DO3"));
        CHECK(&u.pin(CDP1852::Terminal::CLOCK) == &u.pin("CLOCK"));
        CHECK(&u.pin(CDP1852::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CDP1852::Terminal::CS2) == &u.pin("CS2"));
        CHECK(&u.pin(CDP1852::Terminal::CLEAR) == &u.pin("CLEAR"));
        CHECK(&u.pin(CDP1852::Terminal::DO4) == &u.pin("DO4"));
        CHECK(&u.pin(CDP1852::Terminal::DI4) == &u.pin("DI4"));
        CHECK(&u.pin(CDP1852::Terminal::DO5) == &u.pin("DO5"));
        CHECK(&u.pin(CDP1852::Terminal::DI5) == &u.pin("DI5"));
        CHECK(&u.pin(CDP1852::Terminal::DO6) == &u.pin("DO6"));
        CHECK(&u.pin(CDP1852::Terminal::DI6) == &u.pin("DI6"));
        CHECK(&u.pin(CDP1852::Terminal::DO7) == &u.pin("DO7"));
        CHECK(&u.pin(CDP1852::Terminal::DI7) == &u.pin("DI7"));
        CHECK(&u.pin(CDP1852::Terminal::nSR_SR) == &u.pin("nSR_SR"));
        CHECK(&u.pin(CDP1852::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("DO outputs are HighZ when unpowered")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        CHECK(u.pin("DO0").read<Level>() == Level::HighZ);
        CHECK(u.pin("DO7").read<Level>() == Level::HighZ);
        CHECK(u.pin("nSR_SR").read<Level>() == Level::HighZ);
    }

    TEST_CASE("DO outputs are HighZ when VDD below range")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(3.0f);

        CHECK(u.pin("DO0").read<Level>() == Level::HighZ);
        CHECK(u.pin("DO7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("DO outputs are HighZ when VDD above range")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(12.0f);

        CHECK(u.pin("DO0").read<Level>() == Level::HighZ);
        CHECK(u.pin("DO7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Input mode: DO HighZ when deselected (nCS1_CS1=Low)")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::High);
        u.pin("nCS1_CS1").drive<Level>(Level::Low); // not selected in input mode

        // latch something first
        u.pin("DI0").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);

        CHECK(u.pin("DO0").read<Level>() == Level::HighZ);
        CHECK(u.pin("DO7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Input mode: DO HighZ when CS2=Low")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::Low);
        u.pin("nCS1_CS1").drive<Level>(Level::High);
        u.pin("CS2").drive<Level>(Level::Low);

        u.pin("DI0").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);

        CHECK(u.pin("DO0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Input mode: CLOCK trailing edge latches DI and sets nSR_SR=Low")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::Low);
        u.pin("nCS1_CS1").drive<Level>(Level::Low); // deselected in input mode
        u.pin("CS2").drive<Level>(Level::Low);

        u.pin("DI0").drive<Level>(Level::High);
        u.pin("DI1").drive<Level>(Level::Low);
        u.pin("DI2").drive<Level>(Level::High);
        u.pin("DI3").drive<Level>(Level::Low);
        u.pin("DI4").drive<Level>(Level::High);
        u.pin("DI5").drive<Level>(Level::Low);
        u.pin("DI6").drive<Level>(Level::High);
        u.pin("DI7").drive<Level>(Level::Low);

        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low); // trailing edge

        // nSR_SR should go Low (data ready)
        CHECK(u.pin("nSR_SR").read<Level>() == Level::Low);

        // now select to read DO
        u.pin("nCS1_CS1").drive<Level>(Level::High);
        u.pin("CS2").drive<Level>(Level::High);

        CHECK(u.pin("DO0").read<Level>() == Level::High);
        CHECK(u.pin("DO1").read<Level>() == Level::Low);
        CHECK(u.pin("DO2").read<Level>() == Level::High);
        CHECK(u.pin("DO3").read<Level>() == Level::Low);
        CHECK(u.pin("DO4").read<Level>() == Level::High);
        CHECK(u.pin("DO5").read<Level>() == Level::Low);
        CHECK(u.pin("DO6").read<Level>() == Level::High);
        CHECK(u.pin("DO7").read<Level>() == Level::Low);

        // nSR_SR=High when selected (data transferred)
        CHECK(u.pin("nSR_SR").read<Level>() == Level::High);
    }

    TEST_CASE("Input mode: rising edge of CLOCK does not latch")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::Low);
        u.pin("nCS1_CS1").drive<Level>(Level::High);
        u.pin("CS2").drive<Level>(Level::High);

        u.pin("DI0").drive<Level>(Level::High);

        // only rising edge -> no latch expected
        u.pin("CLOCK").drive<Level>(Level::Low);
        u.pin("CLOCK").drive<Level>(Level::High);

        // register untouched -> DO0 reflects 0 from initial register
        CHECK(u.pin("DO0").read<Level>() == Level::Low);
    }

    TEST_CASE("Output mode: DO always driven from register regardless of CS")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::High);
        u.pin("nCS1_CS1").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::High);

        u.pin("DI0").drive<Level>(Level::High);
        u.pin("DI1").drive<Level>(Level::Low);
        u.pin("DI2").drive<Level>(Level::High);
        u.pin("DI3").drive<Level>(Level::Low);
        u.pin("DI4").drive<Level>(Level::High);
        u.pin("DI5").drive<Level>(Level::Low);
        u.pin("DI6").drive<Level>(Level::High);
        u.pin("DI7").drive<Level>(Level::Low);

        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low); // latch

        // deselect
        u.pin("nCS1_CS1").drive<Level>(Level::High);
        u.pin("CS2").drive<Level>(Level::Low);

        // DO still driven
        CHECK(u.pin("DO0").read<Level>() == Level::High);
        CHECK(u.pin("DO1").read<Level>() == Level::Low);
        CHECK(u.pin("DO2").read<Level>() == Level::High);
        CHECK(u.pin("DO3").read<Level>() == Level::Low);
        CHECK(u.pin("DO4").read<Level>() == Level::High);
        CHECK(u.pin("DO5").read<Level>() == Level::Low);
        CHECK(u.pin("DO6").read<Level>() == Level::High);
        CHECK(u.pin("DO7").read<Level>() == Level::Low);
    }

    TEST_CASE("Output mode: CLOCK latch only when nCS1_CS1=Low AND CS2=High")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::High);

        // first write with wrong CS -> should not latch
        u.pin("nCS1_CS1").drive<Level>(Level::High); // wrong: CS1/CS1=High means not selected in output mode
        u.pin("CS2").drive<Level>(Level::High);
        u.pin("DI0").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);
        CHECK(u.pin("DO0").read<Level>() == Level::Low); // register unchanged

        // now with correct CS
        u.pin("nCS1_CS1").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);
        CHECK(u.pin("DO0").read<Level>() == Level::High);
    }

    TEST_CASE("Output mode: nSR_SR=Low when selected, High when deselected")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::High);
        u.pin("nCS1_CS1").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::High);

        CHECK(u.pin("nSR_SR").read<Level>() == Level::Low); // selected

        u.pin("nCS1_CS1").drive<Level>(Level::High);

        CHECK(u.pin("nSR_SR").read<Level>() == Level::High); // deselected
    }

    TEST_CASE("CLEAR=High resets register in input mode")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::Low);
        u.pin("nCS1_CS1").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::Low);

        // latch 0xFF
        u.pin("DI0").drive<Level>(Level::High);
        u.pin("DI1").drive<Level>(Level::High);
        u.pin("DI2").drive<Level>(Level::High);
        u.pin("DI3").drive<Level>(Level::High);
        u.pin("DI4").drive<Level>(Level::High);
        u.pin("DI5").drive<Level>(Level::High);
        u.pin("DI6").drive<Level>(Level::High);
        u.pin("DI7").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);

        // select to verify latch
        u.pin("nCS1_CS1").drive<Level>(Level::High);
        u.pin("CS2").drive<Level>(Level::High);
        CHECK(u.pin("DO0").read<Level>() == Level::High);

        // CLEAR
        u.pin("CLEAR").drive<Level>(Level::High);

        CHECK(u.pin("DO0").read<Level>() == Level::Low);
        CHECK(u.pin("DO7").read<Level>() == Level::Low);
        // input mode after CLEAR: nSR_SR=High (inactive)
        u.pin("nCS1_CS1").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::Low);
        CHECK(u.pin("nSR_SR").read<Level>() == Level::High);
    }

    TEST_CASE("CLEAR=High resets register in output mode")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::High);
        u.pin("nCS1_CS1").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::High);

        u.pin("DI0").drive<Level>(Level::High);
        u.pin("DI7").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);
        CHECK(u.pin("DO0").read<Level>() == Level::High);

        u.pin("CLEAR").drive<Level>(Level::High);

        CHECK(u.pin("DO0").read<Level>() == Level::Low);
        CHECK(u.pin("DO7").read<Level>() == Level::Low);
    }

    TEST_CASE("reset() clears register and releases DO when unpowered")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::High);
        u.pin("nCS1_CS1").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::High);

        u.pin("DI0").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);
        CHECK(u.pin("DO0").read<Level>() == Level::High);

        u.reset();

        CHECK(u.pin("DO0").read<Level>() == Level::Low);
        CHECK(u.pin("DO7").read<Level>() == Level::Low);
    }

    TEST_CASE("reset() in input mode: nSR_SR=High (inactive)")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::Low);
        u.pin("nCS1_CS1").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::Low);

        // latch so nSR_SR goes Low
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);
        CHECK(u.pin("nSR_SR").read<Level>() == Level::Low);

        u.reset();

        CHECK(u.pin("nSR_SR").read<Level>() == Level::High);
    }

    TEST_CASE("VDD removal releases DO and nSR_SR")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MODE").drive<Level>(Level::High);
        u.pin("nCS1_CS1").drive<Level>(Level::Low);
        u.pin("CS2").drive<Level>(Level::High);

        u.pin("DI0").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);
        CHECK(u.pin("DO0").read<Level>() == Level::High);

        u.pin("VDD").release();

        CHECK(u.pin("DO0").read<Level>() == Level::HighZ);
        CHECK(u.pin("DO7").read<Level>() == Level::HighZ);
        CHECK(u.pin("nSR_SR").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name matches descriptor")
    {
        CDP1852 u{ CDP1852::Descriptor{ .name = "U3" } };

        CHECK(std::string_view(u.name()) == "U3");
    }
}
