#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::memory::cdp1xxx;
using namespace nfx::silicon::signal;

namespace
{
    CDP1832::Descriptor makeDescriptor(const char* name = "U10")
    {
        CDP1832::Descriptor d;
        d.name = name;
        d.rom.fill(0x00);
        return d;
    }
} // namespace

TEST_SUITE("memory::cdp1xxx::CDP1832")
{
    TEST_CASE("Construction creates 24 pins")
    {
        CDP1832 u{ makeDescriptor() };

        CHECK(u.pins().size() == 24);
    }

    TEST_CASE("Pin names match DIP-24 datasheet")
    {
        CDP1832 u{ makeDescriptor() };

        CHECK(u.pin("MA7").descriptor().name == std::string_view("MA7"));
        CHECK(u.pin("MA6").descriptor().name == std::string_view("MA6"));
        CHECK(u.pin("MA5").descriptor().name == std::string_view("MA5"));
        CHECK(u.pin("MA4").descriptor().name == std::string_view("MA4"));
        CHECK(u.pin("MA3").descriptor().name == std::string_view("MA3"));
        CHECK(u.pin("MA2").descriptor().name == std::string_view("MA2"));
        CHECK(u.pin("MA1").descriptor().name == std::string_view("MA1"));
        CHECK(u.pin("MA0").descriptor().name == std::string_view("MA0"));
        CHECK(u.pin("BUS0").descriptor().name == std::string_view("BUS0"));
        CHECK(u.pin("BUS1").descriptor().name == std::string_view("BUS1"));
        CHECK(u.pin("BUS2").descriptor().name == std::string_view("BUS2"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("BUS3").descriptor().name == std::string_view("BUS3"));
        CHECK(u.pin("BUS4").descriptor().name == std::string_view("BUS4"));
        CHECK(u.pin("BUS5").descriptor().name == std::string_view("BUS5"));
        CHECK(u.pin("BUS6").descriptor().name == std::string_view("BUS6"));
        CHECK(u.pin("BUS7").descriptor().name == std::string_view("BUS7"));
        CHECK(u.pin("NC18").descriptor().name == std::string_view("NC18"));
        CHECK(u.pin("NC19").descriptor().name == std::string_view("NC19"));
        CHECK(u.pin("CS").descriptor().name == std::string_view("CS"));
        CHECK(u.pin("NC21").descriptor().name == std::string_view("NC21"));
        CHECK(u.pin("NC22").descriptor().name == std::string_view("NC22"));
        CHECK(u.pin("A8").descriptor().name == std::string_view("A8"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CDP1832 u{ makeDescriptor() };

        CHECK(&u.pin(CDP1832::Terminal::MA7) == &u.pin("MA7"));
        CHECK(&u.pin(CDP1832::Terminal::MA6) == &u.pin("MA6"));
        CHECK(&u.pin(CDP1832::Terminal::MA5) == &u.pin("MA5"));
        CHECK(&u.pin(CDP1832::Terminal::MA4) == &u.pin("MA4"));
        CHECK(&u.pin(CDP1832::Terminal::MA3) == &u.pin("MA3"));
        CHECK(&u.pin(CDP1832::Terminal::MA2) == &u.pin("MA2"));
        CHECK(&u.pin(CDP1832::Terminal::MA1) == &u.pin("MA1"));
        CHECK(&u.pin(CDP1832::Terminal::MA0) == &u.pin("MA0"));
        CHECK(&u.pin(CDP1832::Terminal::BUS0) == &u.pin("BUS0"));
        CHECK(&u.pin(CDP1832::Terminal::BUS1) == &u.pin("BUS1"));
        CHECK(&u.pin(CDP1832::Terminal::BUS2) == &u.pin("BUS2"));
        CHECK(&u.pin(CDP1832::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CDP1832::Terminal::BUS3) == &u.pin("BUS3"));
        CHECK(&u.pin(CDP1832::Terminal::BUS4) == &u.pin("BUS4"));
        CHECK(&u.pin(CDP1832::Terminal::BUS5) == &u.pin("BUS5"));
        CHECK(&u.pin(CDP1832::Terminal::BUS6) == &u.pin("BUS6"));
        CHECK(&u.pin(CDP1832::Terminal::BUS7) == &u.pin("BUS7"));
        CHECK(&u.pin(CDP1832::Terminal::NC18) == &u.pin("NC18"));
        CHECK(&u.pin(CDP1832::Terminal::NC19) == &u.pin("NC19"));
        CHECK(&u.pin(CDP1832::Terminal::CS) == &u.pin("CS"));
        CHECK(&u.pin(CDP1832::Terminal::NC21) == &u.pin("NC21"));
        CHECK(&u.pin(CDP1832::Terminal::NC22) == &u.pin("NC22"));
        CHECK(&u.pin(CDP1832::Terminal::A8) == &u.pin("A8"));
        CHECK(&u.pin(CDP1832::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("BUS outputs are HighZ when unpowered")
    {
        CDP1832 u{ makeDescriptor() };

        CHECK(u.pin("BUS0").read<Level>() == Level::HighZ);
        CHECK(u.pin("BUS7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("BUS outputs are HighZ when VDD below range")
    {
        CDP1832 u{ makeDescriptor() };

        u.pin("VDD").drive<float>(3.0f);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("MA0").drive<Level>(Level::Low);
        u.pin("MA1").drive<Level>(Level::Low);
        u.pin("MA2").drive<Level>(Level::Low);
        u.pin("MA3").drive<Level>(Level::Low);
        u.pin("MA4").drive<Level>(Level::Low);
        u.pin("MA5").drive<Level>(Level::Low);
        u.pin("MA6").drive<Level>(Level::Low);
        u.pin("MA7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);

        CHECK(u.pin("BUS0").read<Level>() == Level::HighZ);
        CHECK(u.pin("BUS7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("BUS outputs are HighZ when VDD above range")
    {
        CDP1832 u{ makeDescriptor() };

        u.pin("VDD").drive<float>(12.0f);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("MA0").drive<Level>(Level::Low);
        u.pin("MA1").drive<Level>(Level::Low);
        u.pin("MA2").drive<Level>(Level::Low);
        u.pin("MA3").drive<Level>(Level::Low);
        u.pin("MA4").drive<Level>(Level::Low);
        u.pin("MA5").drive<Level>(Level::Low);
        u.pin("MA6").drive<Level>(Level::Low);
        u.pin("MA7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);

        CHECK(u.pin("BUS0").read<Level>() == Level::HighZ);
        CHECK(u.pin("BUS7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("BUS outputs are HighZ when CS is High")
    {
        auto d = makeDescriptor();
        d.rom[0] = 0xFF;
        CDP1832 u{ d };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("CS").drive<Level>(Level::High);
        u.pin("MA0").drive<Level>(Level::Low);
        u.pin("MA1").drive<Level>(Level::Low);
        u.pin("MA2").drive<Level>(Level::Low);
        u.pin("MA3").drive<Level>(Level::Low);
        u.pin("MA4").drive<Level>(Level::Low);
        u.pin("MA5").drive<Level>(Level::Low);
        u.pin("MA6").drive<Level>(Level::Low);
        u.pin("MA7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);

        CHECK(u.pin("BUS0").read<Level>() == Level::HighZ);
        CHECK(u.pin("BUS7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("BUS outputs are HighZ when CS is HighZ")
    {
        auto d = makeDescriptor();
        d.rom[0] = 0xFF;
        CDP1832 u{ d };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("MA0").drive<Level>(Level::Low);
        u.pin("MA1").drive<Level>(Level::Low);
        u.pin("MA2").drive<Level>(Level::Low);
        u.pin("MA3").drive<Level>(Level::Low);
        u.pin("MA4").drive<Level>(Level::Low);
        u.pin("MA5").drive<Level>(Level::Low);
        u.pin("MA6").drive<Level>(Level::Low);
        u.pin("MA7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);

        CHECK(u.pin("BUS0").read<Level>() == Level::HighZ);
        CHECK(u.pin("BUS7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Reads address 0 correctly")
    {
        auto d = makeDescriptor();
        d.rom[0] = 0xA5; // 1010 0101
        CDP1832 u{ d };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("MA0").drive<Level>(Level::Low);
        u.pin("MA1").drive<Level>(Level::Low);
        u.pin("MA2").drive<Level>(Level::Low);
        u.pin("MA3").drive<Level>(Level::Low);
        u.pin("MA4").drive<Level>(Level::Low);
        u.pin("MA5").drive<Level>(Level::Low);
        u.pin("MA6").drive<Level>(Level::Low);
        u.pin("MA7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);

        // 0xA5 = 1010 0101
        CHECK(u.pin("BUS0").read<Level>() == Level::High);
        CHECK(u.pin("BUS1").read<Level>() == Level::Low);
        CHECK(u.pin("BUS2").read<Level>() == Level::High);
        CHECK(u.pin("BUS3").read<Level>() == Level::Low);
        CHECK(u.pin("BUS4").read<Level>() == Level::Low);
        CHECK(u.pin("BUS5").read<Level>() == Level::High);
        CHECK(u.pin("BUS6").read<Level>() == Level::Low);
        CHECK(u.pin("BUS7").read<Level>() == Level::High);
    }

    TEST_CASE("Reads address 0x1FF correctly")
    {
        auto d = makeDescriptor();
        d.rom[0x1FF] = 0x3C; // 0011 1100
        CDP1832 u{ d };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("MA0").drive<Level>(Level::High);
        u.pin("MA1").drive<Level>(Level::High);
        u.pin("MA2").drive<Level>(Level::High);
        u.pin("MA3").drive<Level>(Level::High);
        u.pin("MA4").drive<Level>(Level::High);
        u.pin("MA5").drive<Level>(Level::High);
        u.pin("MA6").drive<Level>(Level::High);
        u.pin("MA7").drive<Level>(Level::High);
        u.pin("A8").drive<Level>(Level::High);

        // 0x3C = 0011 1100
        CHECK(u.pin("BUS0").read<Level>() == Level::Low);
        CHECK(u.pin("BUS1").read<Level>() == Level::Low);
        CHECK(u.pin("BUS2").read<Level>() == Level::High);
        CHECK(u.pin("BUS3").read<Level>() == Level::High);
        CHECK(u.pin("BUS4").read<Level>() == Level::High);
        CHECK(u.pin("BUS5").read<Level>() == Level::High);
        CHECK(u.pin("BUS6").read<Level>() == Level::Low);
        CHECK(u.pin("BUS7").read<Level>() == Level::Low);
    }

    TEST_CASE("Address changes update BUS reactively")
    {
        auto d = makeDescriptor();
        d.rom[0] = 0x00;
        d.rom[1] = 0xFF;
        CDP1832 u{ d };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("MA0").drive<Level>(Level::Low);
        u.pin("MA1").drive<Level>(Level::Low);
        u.pin("MA2").drive<Level>(Level::Low);
        u.pin("MA3").drive<Level>(Level::Low);
        u.pin("MA4").drive<Level>(Level::Low);
        u.pin("MA5").drive<Level>(Level::Low);
        u.pin("MA6").drive<Level>(Level::Low);
        u.pin("MA7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);

        CHECK(u.pin("BUS0").read<Level>() == Level::Low);

        u.pin("MA0").drive<Level>(Level::High); // addr 1

        CHECK(u.pin("BUS0").read<Level>() == Level::High);
        CHECK(u.pin("BUS7").read<Level>() == Level::High);
    }

    TEST_CASE("CS toggle releases and restores BUS")
    {
        auto d = makeDescriptor();
        d.rom[0] = 0xFF;
        CDP1832 u{ d };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("MA0").drive<Level>(Level::Low);
        u.pin("MA1").drive<Level>(Level::Low);
        u.pin("MA2").drive<Level>(Level::Low);
        u.pin("MA3").drive<Level>(Level::Low);
        u.pin("MA4").drive<Level>(Level::Low);
        u.pin("MA5").drive<Level>(Level::Low);
        u.pin("MA6").drive<Level>(Level::Low);
        u.pin("MA7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);

        CHECK(u.pin("BUS0").read<Level>() == Level::High);

        u.pin("CS").drive<Level>(Level::High);
        CHECK(u.pin("BUS0").read<Level>() == Level::HighZ);

        u.pin("CS").drive<Level>(Level::Low);
        CHECK(u.pin("BUS0").read<Level>() == Level::High);
    }

    TEST_CASE("VDD removed releases BUS")
    {
        auto d = makeDescriptor();
        d.rom[0] = 0xFF;
        CDP1832 u{ d };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("CS").drive<Level>(Level::Low);
        u.pin("MA0").drive<Level>(Level::Low);
        u.pin("MA1").drive<Level>(Level::Low);
        u.pin("MA2").drive<Level>(Level::Low);
        u.pin("MA3").drive<Level>(Level::Low);
        u.pin("MA4").drive<Level>(Level::Low);
        u.pin("MA5").drive<Level>(Level::Low);
        u.pin("MA6").drive<Level>(Level::Low);
        u.pin("MA7").drive<Level>(Level::Low);
        u.pin("A8").drive<Level>(Level::Low);

        CHECK(u.pin("BUS0").read<Level>() == Level::High);

        u.pin("VDD").release();
        CHECK(u.pin("BUS0").read<Level>() == Level::HighZ);
        CHECK(u.pin("BUS7").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Component name matches descriptor")
    {
        CDP1832 u{ makeDescriptor("U10") };

        CHECK(std::string_view(u.name()) == "U10");
    }
}
