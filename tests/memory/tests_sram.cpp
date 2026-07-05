#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::memory::sram;
using namespace nfx::silicon::signal;

TEST_SUITE("memory::sram::I2114")
{
    TEST_CASE("Construction creates 18 pins")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        CHECK(u.pins().size() == 18);
    }

    TEST_CASE("Pin names match DIP-18 datasheet")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        CHECK(u.pin("A6").descriptor().name == std::string_view("A6"));
        CHECK(u.pin("A5").descriptor().name == std::string_view("A5"));
        CHECK(u.pin("A4").descriptor().name == std::string_view("A4"));
        CHECK(u.pin("A3").descriptor().name == std::string_view("A3"));
        CHECK(u.pin("A0").descriptor().name == std::string_view("A0"));
        CHECK(u.pin("A1").descriptor().name == std::string_view("A1"));
        CHECK(u.pin("A2").descriptor().name == std::string_view("A2"));
        CHECK(u.pin("CS").descriptor().name == std::string_view("CS"));
        CHECK(u.pin("GND").descriptor().name == std::string_view("GND"));

        CHECK(u.pin("WE").descriptor().name == std::string_view("WE"));
        CHECK(u.pin("IO4").descriptor().name == std::string_view("IO4"));
        CHECK(u.pin("IO3").descriptor().name == std::string_view("IO3"));
        CHECK(u.pin("IO2").descriptor().name == std::string_view("IO2"));
        CHECK(u.pin("IO1").descriptor().name == std::string_view("IO1"));
        CHECK(u.pin("A9").descriptor().name == std::string_view("A9"));
        CHECK(u.pin("A8").descriptor().name == std::string_view("A8"));
        CHECK(u.pin("A7").descriptor().name == std::string_view("A7"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        CHECK(&u.pin(I2114::Terminal::A6) == &u.pin("A6"));
        CHECK(&u.pin(I2114::Terminal::A5) == &u.pin("A5"));
        CHECK(&u.pin(I2114::Terminal::A4) == &u.pin("A4"));
        CHECK(&u.pin(I2114::Terminal::A3) == &u.pin("A3"));
        CHECK(&u.pin(I2114::Terminal::A0) == &u.pin("A0"));
        CHECK(&u.pin(I2114::Terminal::A1) == &u.pin("A1"));
        CHECK(&u.pin(I2114::Terminal::A2) == &u.pin("A2"));
        CHECK(&u.pin(I2114::Terminal::CS) == &u.pin("CS"));
        CHECK(&u.pin(I2114::Terminal::GND) == &u.pin("GND"));

        CHECK(&u.pin(I2114::Terminal::WE) == &u.pin("WE"));
        CHECK(&u.pin(I2114::Terminal::IO4) == &u.pin("IO4"));
        CHECK(&u.pin(I2114::Terminal::IO3) == &u.pin("IO3"));
        CHECK(&u.pin(I2114::Terminal::IO2) == &u.pin("IO2"));
        CHECK(&u.pin(I2114::Terminal::IO1) == &u.pin("IO1"));
        CHECK(&u.pin(I2114::Terminal::A9) == &u.pin("A9"));
        CHECK(&u.pin(I2114::Terminal::A8) == &u.pin("A8"));
        CHECK(&u.pin(I2114::Terminal::A7) == &u.pin("A7"));
        CHECK(&u.pin(I2114::Terminal::VCC) == &u.pin("VCC"));
    }

    TEST_CASE("I/O outputs are HighZ when unpowered")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        CHECK(u.pin("IO1").read<Level>() == Level::HighZ);
        CHECK(u.pin("IO4").read<Level>() == Level::HighZ);
    }

    TEST_CASE("I/O outputs are HighZ when VCC below range")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        u.pin("VCC").drive<float>(4.0f);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("WE").drive<Level>(Level::High);
        u.pin("A0").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("A2").drive<Level>(Level::Low);
        u.pin("A3").drive<Level>(Level::Low);
        u.pin("A4").drive<Level>(Level::Low);
        u.pin("A5").drive<Level>(Level::Low);
        u.pin("A6").drive<Level>(Level::Low);
        u.pin("A7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);
        u.pin("A9").drive<Level>(Level::Low);

        CHECK(u.pin("IO1").read<Level>() == Level::HighZ);
        CHECK(u.pin("IO4").read<Level>() == Level::HighZ);
    }

    TEST_CASE("I/O outputs are HighZ when VCC above range")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        u.pin("VCC").drive<float>(6.0f);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("WE").drive<Level>(Level::High);
        u.pin("A0").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("A2").drive<Level>(Level::Low);
        u.pin("A3").drive<Level>(Level::Low);
        u.pin("A4").drive<Level>(Level::Low);
        u.pin("A5").drive<Level>(Level::Low);
        u.pin("A6").drive<Level>(Level::Low);
        u.pin("A7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);
        u.pin("A9").drive<Level>(Level::Low);

        CHECK(u.pin("IO1").read<Level>() == Level::HighZ);
        CHECK(u.pin("IO4").read<Level>() == Level::HighZ);
    }

    TEST_CASE("I/O outputs are HighZ when CS is High")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        u.pin("VCC").drive<float>(5.0f);
        u.pin("CS").drive<Level>(Level::High);
        u.pin("WE").drive<Level>(Level::High);
        u.pin("A0").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("A2").drive<Level>(Level::Low);
        u.pin("A3").drive<Level>(Level::Low);
        u.pin("A4").drive<Level>(Level::Low);
        u.pin("A5").drive<Level>(Level::Low);
        u.pin("A6").drive<Level>(Level::Low);
        u.pin("A7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);
        u.pin("A9").drive<Level>(Level::Low);

        CHECK(u.pin("IO1").read<Level>() == Level::HighZ);
        CHECK(u.pin("IO4").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Write then read at address 0")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        u.pin("VCC").drive<float>(5.0f);
        u.pin("A0").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("A2").drive<Level>(Level::Low);
        u.pin("A3").drive<Level>(Level::Low);
        u.pin("A4").drive<Level>(Level::Low);
        u.pin("A5").drive<Level>(Level::Low);
        u.pin("A6").drive<Level>(Level::Low);
        u.pin("A7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);
        u.pin("A9").drive<Level>(Level::Low);

        u.pin("CS").drive<Level>(Level::Low);
        u.pin("WE").drive<Level>(Level::Low);

        // write 0b1010
        u.pin("IO1").drive<Level>(Level::Low);
        u.pin("IO2").drive<Level>(Level::High);
        u.pin("IO3").drive<Level>(Level::Low);
        u.pin("IO4").drive<Level>(Level::High);

        u.pin("WE").drive<Level>(Level::High);

        CHECK(u.pin("IO1").read<Level>() == Level::Low);
        CHECK(u.pin("IO2").read<Level>() == Level::High);
        CHECK(u.pin("IO3").read<Level>() == Level::Low);
        CHECK(u.pin("IO4").read<Level>() == Level::High);
    }

    TEST_CASE("Write then read at address 0x3FF")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        u.pin("VCC").drive<float>(5.0f);
        u.pin("A0").drive<Level>(Level::High);
        u.pin("A1").drive<Level>(Level::High);
        u.pin("A2").drive<Level>(Level::High);
        u.pin("A3").drive<Level>(Level::High);
        u.pin("A4").drive<Level>(Level::High);
        u.pin("A5").drive<Level>(Level::High);
        u.pin("A6").drive<Level>(Level::High);
        u.pin("A7").drive<Level>(Level::High);
        u.pin("A8").drive<Level>(Level::High);
        u.pin("A9").drive<Level>(Level::High);

        u.pin("CS").drive<Level>(Level::Low);
        u.pin("WE").drive<Level>(Level::Low);

        // write 0b0101
        u.pin("IO1").drive<Level>(Level::High);
        u.pin("IO2").drive<Level>(Level::Low);
        u.pin("IO3").drive<Level>(Level::High);
        u.pin("IO4").drive<Level>(Level::Low);

        u.pin("WE").drive<Level>(Level::High);

        CHECK(u.pin("IO1").read<Level>() == Level::High);
        CHECK(u.pin("IO2").read<Level>() == Level::Low);
        CHECK(u.pin("IO3").read<Level>() == Level::High);
        CHECK(u.pin("IO4").read<Level>() == Level::Low);
    }

    TEST_CASE("Two addresses are independent")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        u.pin("VCC").drive<float>(5.0f);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("A2").drive<Level>(Level::Low);
        u.pin("A3").drive<Level>(Level::Low);
        u.pin("A4").drive<Level>(Level::Low);
        u.pin("A5").drive<Level>(Level::Low);
        u.pin("A6").drive<Level>(Level::Low);
        u.pin("A7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);
        u.pin("A9").drive<Level>(Level::Low);

        // write 0xF at addr 0
        u.pin("A0").drive<Level>(Level::Low);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("WE").drive<Level>(Level::Low);

        u.pin("IO1").drive<Level>(Level::High);
        u.pin("IO2").drive<Level>(Level::High);
        u.pin("IO3").drive<Level>(Level::High);
        u.pin("IO4").drive<Level>(Level::High);
        u.pin("WE").drive<Level>(Level::High);

        // write 0x0 at addr 1 (CS stays Low the whole time)
        u.pin("A0").drive<Level>(Level::High);
        u.pin("WE").drive<Level>(Level::Low);
        u.pin("IO1").drive<Level>(Level::Low);
        u.pin("IO2").drive<Level>(Level::Low);
        u.pin("IO3").drive<Level>(Level::Low);
        u.pin("IO4").drive<Level>(Level::Low);
        u.pin("WE").drive<Level>(Level::High);

        // read addr 0
        u.pin("A0").drive<Level>(Level::Low);
        CHECK(u.pin("IO1").read<Level>() == Level::High);
        CHECK(u.pin("IO2").read<Level>() == Level::High);
        CHECK(u.pin("IO3").read<Level>() == Level::High);
        CHECK(u.pin("IO4").read<Level>() == Level::High);

        // read addr 1
        u.pin("A0").drive<Level>(Level::High);
        CHECK(u.pin("IO1").read<Level>() == Level::Low);
        CHECK(u.pin("IO2").read<Level>() == Level::Low);
        CHECK(u.pin("IO3").read<Level>() == Level::Low);
        CHECK(u.pin("IO4").read<Level>() == Level::Low);
    }

    TEST_CASE("reset() clears RAM and releases I/O")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        u.pin("VCC").drive<float>(5.0f);
        u.pin("A0").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("A2").drive<Level>(Level::Low);
        u.pin("A3").drive<Level>(Level::Low);
        u.pin("A4").drive<Level>(Level::Low);
        u.pin("A5").drive<Level>(Level::Low);
        u.pin("A6").drive<Level>(Level::Low);
        u.pin("A7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);
        u.pin("A9").drive<Level>(Level::Low);

        u.pin("CS").drive<Level>(Level::Low);
        u.pin("WE").drive<Level>(Level::Low);

        u.pin("IO1").drive<Level>(Level::High);
        u.pin("IO2").drive<Level>(Level::High);
        u.pin("IO3").drive<Level>(Level::High);
        u.pin("IO4").drive<Level>(Level::High);
        u.pin("WE").drive<Level>(Level::High);

        CHECK(u.pin("IO1").read<Level>() == Level::High);

        u.reset();

        CHECK(u.pin("IO1").read<Level>() == Level::HighZ);

        u.pin("WE").release();
        u.pin("WE").drive<Level>(Level::High);
        CHECK(u.pin("IO1").read<Level>() == Level::Low);
        CHECK(u.pin("IO2").read<Level>() == Level::Low);
        CHECK(u.pin("IO3").read<Level>() == Level::Low);
        CHECK(u.pin("IO4").read<Level>() == Level::Low);
    }

    TEST_CASE("VCC removed releases I/O")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        u.pin("VCC").drive<float>(5.0f);
        u.pin("A0").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("A2").drive<Level>(Level::Low);
        u.pin("A3").drive<Level>(Level::Low);
        u.pin("A4").drive<Level>(Level::Low);
        u.pin("A5").drive<Level>(Level::Low);
        u.pin("A6").drive<Level>(Level::Low);
        u.pin("A7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);
        u.pin("A9").drive<Level>(Level::Low);

        u.pin("CS").drive<Level>(Level::Low);
        u.pin("WE").drive<Level>(Level::Low);

        u.pin("IO1").drive<Level>(Level::High);
        u.pin("IO2").drive<Level>(Level::High);
        u.pin("IO3").drive<Level>(Level::High);
        u.pin("IO4").drive<Level>(Level::High);
        u.pin("WE").drive<Level>(Level::High);

        CHECK(u.pin("IO1").read<Level>() == Level::High);

        u.pin("VCC").release();

        CHECK(u.pin("IO1").read<Level>() == Level::HighZ);
        CHECK(u.pin("IO4").read<Level>() == Level::HighZ);
    }

    TEST_CASE("WE floating (HighZ) does not force outputs")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        u.pin("VCC").drive<float>(5.0f);
        u.pin("A0").drive<Level>(Level::Low);
        u.pin("A1").drive<Level>(Level::Low);
        u.pin("A2").drive<Level>(Level::Low);
        u.pin("A3").drive<Level>(Level::Low);
        u.pin("A4").drive<Level>(Level::Low);
        u.pin("A5").drive<Level>(Level::Low);
        u.pin("A6").drive<Level>(Level::Low);
        u.pin("A7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);
        u.pin("A9").drive<Level>(Level::Low);

        // CS Low with WE never driven (still HighZ) must not be treated as a read
        u.pin("CS").drive<Level>(Level::Low);

        CHECK(u.pin("IO1").read<Level>() == Level::HighZ);
        CHECK(u.pin("IO4").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name matches descriptor")
    {
        I2114 u{ I2114::Descriptor{ .name = "U16" } };

        CHECK(std::string_view(u.name()) == "U16");
    }
}
