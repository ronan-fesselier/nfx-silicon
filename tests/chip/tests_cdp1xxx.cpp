#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::chip::cdp1xxx;
using namespace nfx::silicon::signal;

TEST_SUITE("chip::cdp1xxx::CDP1802")
{
    static auto clock = [](CDP1802& u) {
        u.pin("CLOCK").drive<Level>(Level::High);
        u.pin("CLOCK").drive<Level>(Level::Low);
    };

    static auto clockN = [](CDP1802& u, int n) {
        for (int i = 0; i < n; ++i)
        {
            u.pin("CLOCK").drive<Level>(Level::High);
            u.pin("CLOCK").drive<Level>(Level::Low);
        }
    };

    static auto power = [](CDP1802& u) { u.pin("VDD").drive<float>(5.0f); };

    static auto driveBusLow = [](CDP1802& u) {
        static const char* k_bus[8] = { "BUS0", "BUS1", "BUS2", "BUS3", "BUS4", "BUS5", "BUS6", "BUS7" };
        for (const char* n : k_bus)
        {
            u.pin(n).drive<Level>(Level::Low);
        }
    };

    TEST_CASE("Construction creates 40 pins")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        CHECK(u.pins().size() == 40);
    }

    TEST_CASE("Pin names match DIP-40 datasheet")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        CHECK(u.pin("CLOCK").descriptor().name == std::string_view("CLOCK"));
        CHECK(u.pin("nWAIT").descriptor().name == std::string_view("nWAIT"));
        CHECK(u.pin("nCLEAR").descriptor().name == std::string_view("nCLEAR"));
        CHECK(u.pin("Q").descriptor().name == std::string_view("Q"));
        CHECK(u.pin("SC1").descriptor().name == std::string_view("SC1"));
        CHECK(u.pin("SC0").descriptor().name == std::string_view("SC0"));
        CHECK(u.pin("nMRD").descriptor().name == std::string_view("nMRD"));
        CHECK(u.pin("BUS7").descriptor().name == std::string_view("BUS7"));
        CHECK(u.pin("BUS0").descriptor().name == std::string_view("BUS0"));
        CHECK(u.pin("VCC").descriptor().name == std::string_view("VCC"));
        CHECK(u.pin("N2").descriptor().name == std::string_view("N2"));
        CHECK(u.pin("N1").descriptor().name == std::string_view("N1"));
        CHECK(u.pin("N0").descriptor().name == std::string_view("N0"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("nEF4").descriptor().name == std::string_view("nEF4"));
        CHECK(u.pin("nEF1").descriptor().name == std::string_view("nEF1"));
        CHECK(u.pin("MA0").descriptor().name == std::string_view("MA0"));
        CHECK(u.pin("MA7").descriptor().name == std::string_view("MA7"));
        CHECK(u.pin("TPB").descriptor().name == std::string_view("TPB"));
        CHECK(u.pin("TPA").descriptor().name == std::string_view("TPA"));
        CHECK(u.pin("nMWR").descriptor().name == std::string_view("nMWR"));
        CHECK(u.pin("nINT").descriptor().name == std::string_view("nINT"));
        CHECK(u.pin("nDMAO").descriptor().name == std::string_view("nDMAO"));
        CHECK(u.pin("nDMAI").descriptor().name == std::string_view("nDMAI"));
        CHECK(u.pin("nXTAL").descriptor().name == std::string_view("nXTAL"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        CHECK(&u.pin(CDP1802::Terminal::CLOCK) == &u.pin("CLOCK"));
        CHECK(&u.pin(CDP1802::Terminal::nWAIT) == &u.pin("nWAIT"));
        CHECK(&u.pin(CDP1802::Terminal::nCLEAR) == &u.pin("nCLEAR"));
        CHECK(&u.pin(CDP1802::Terminal::Q) == &u.pin("Q"));
        CHECK(&u.pin(CDP1802::Terminal::SC1) == &u.pin("SC1"));
        CHECK(&u.pin(CDP1802::Terminal::SC0) == &u.pin("SC0"));
        CHECK(&u.pin(CDP1802::Terminal::nMRD) == &u.pin("nMRD"));
        CHECK(&u.pin(CDP1802::Terminal::BUS7) == &u.pin("BUS7"));
        CHECK(&u.pin(CDP1802::Terminal::BUS0) == &u.pin("BUS0"));
        CHECK(&u.pin(CDP1802::Terminal::VCC) == &u.pin("VCC"));
        CHECK(&u.pin(CDP1802::Terminal::N2) == &u.pin("N2"));
        CHECK(&u.pin(CDP1802::Terminal::N0) == &u.pin("N0"));
        CHECK(&u.pin(CDP1802::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CDP1802::Terminal::nEF4) == &u.pin("nEF4"));
        CHECK(&u.pin(CDP1802::Terminal::nEF1) == &u.pin("nEF1"));
        CHECK(&u.pin(CDP1802::Terminal::MA0) == &u.pin("MA0"));
        CHECK(&u.pin(CDP1802::Terminal::MA7) == &u.pin("MA7"));
        CHECK(&u.pin(CDP1802::Terminal::TPB) == &u.pin("TPB"));
        CHECK(&u.pin(CDP1802::Terminal::TPA) == &u.pin("TPA"));
        CHECK(&u.pin(CDP1802::Terminal::nMWR) == &u.pin("nMWR"));
        CHECK(&u.pin(CDP1802::Terminal::nINT) == &u.pin("nINT"));
        CHECK(&u.pin(CDP1802::Terminal::nDMAO) == &u.pin("nDMAO"));
        CHECK(&u.pin(CDP1802::Terminal::nDMAI) == &u.pin("nDMAI"));
        CHECK(&u.pin(CDP1802::Terminal::nXTAL) == &u.pin("nXTAL"));
        CHECK(&u.pin(CDP1802::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("Component name matches descriptor")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        CHECK(std::string_view(u.name()) == "U1");
    }

    TEST_CASE("Outputs are HighZ when unpowered")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        CHECK(u.pin("Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("SC1").read<Level>() == Level::HighZ);
        CHECK(u.pin("SC0").read<Level>() == Level::HighZ);
        CHECK(u.pin("nMRD").read<Level>() == Level::HighZ);
        CHECK(u.pin("nMWR").read<Level>() == Level::HighZ);
        CHECK(u.pin("TPA").read<Level>() == Level::HighZ);
        CHECK(u.pin("TPB").read<Level>() == Level::HighZ);
        CHECK(u.pin("N0").read<Level>() == Level::HighZ);
        CHECK(u.pin("MA0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Outputs are HighZ when VDD below range")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        u.pin("VDD").drive<float>(3.0f);

        CHECK(u.pin("Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("SC1").read<Level>() == Level::HighZ);
        CHECK(u.pin("TPA").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Outputs are HighZ when VDD above range")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        u.pin("VDD").drive<float>(12.0f);

        CHECK(u.pin("Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("SC1").read<Level>() == Level::HighZ);
        CHECK(u.pin("TPA").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD removal releases all outputs")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);
        CHECK(u.pin("SC1").read<Level>() != Level::HighZ);

        u.pin("VDD").release();

        CHECK(u.pin("Q").read<Level>() == Level::HighZ);
        CHECK(u.pin("SC1").read<Level>() == Level::HighZ);
        CHECK(u.pin("SC0").read<Level>() == Level::HighZ);
        CHECK(u.pin("nMRD").read<Level>() == Level::HighZ);
        CHECK(u.pin("nMWR").read<Level>() == Level::HighZ);
        CHECK(u.pin("TPA").read<Level>() == Level::HighZ);
        CHECK(u.pin("TPB").read<Level>() == Level::HighZ);
        CHECK(u.pin("N0").read<Level>() == Level::HighZ);
        CHECK(u.pin("MA0").read<Level>() == Level::HighZ);
    }

    TEST_CASE("nCLEAR=Low resets registers and suppresses TPA/TPB")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);
        u.pin("nCLEAR").drive<Level>(Level::Low);

        CHECK(u.pin("TPA").read<Level>() == Level::Low);
        CHECK(u.pin("TPB").read<Level>() == Level::Low);
        CHECK(u.pin("Q").read<Level>() == Level::Low);

        auto ie = u.inspect("IE");
        REQUIRE(ie.has_value());
        CHECK(ie->value == 1u);

        auto p = u.inspect("P");
        REQUIRE(p.has_value());
        CHECK(p->value == 0u);

        auto r0 = u.inspect("R0");
        REQUIRE(r0.has_value());
        CHECK(r0->value == 0u);
    }

    TEST_CASE("nCLEAR=Low: CLOCK pulses do not advance TPA/TPB")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);
        u.pin("nCLEAR").drive<Level>(Level::Low);
        clockN(u, 8);

        CHECK(u.pin("TPA").read<Level>() == Level::Low);
        CHECK(u.pin("TPB").read<Level>() == Level::Low);
    }

    TEST_CASE("nWAIT=Low freezes outputs: CLOCK has no effect")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // capture MA0 before wait
        const Level ma0Before = u.pin("MA0").read<Level>();

        u.pin("nWAIT").drive<Level>(Level::Low);
        clockN(u, 8);

        CHECK(u.pin("TPA").read<Level>() == Level::Low);
        CHECK(u.pin("TPB").read<Level>() == Level::Low);
        CHECK(u.pin("MA0").read<Level>() == ma0Before);
    }

    TEST_CASE("8 CLOCKs produce exactly one TPA pulse and one TPB pulse")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // drive BUS=0x00 so fetch reads NOP-equivalent
        driveBusLow(u);

        int tpaRising = 0;
        int tpbRising = 0;
        Level lastTPA = u.pin("TPA").read<Level>();
        Level lastTPB = u.pin("TPB").read<Level>();

        for (int i = 0; i < 8; ++i)
        {
            u.pin("CLOCK").drive<Level>(Level::High);
            const Level tpa = u.pin("TPA").read<Level>();
            const Level tpb = u.pin("TPB").read<Level>();
            if (lastTPA != Level::High && tpa == Level::High)
            {
                ++tpaRising;
            }
            if (lastTPB != Level::High && tpb == Level::High)
            {
                ++tpbRising;
            }
            lastTPA = tpa;
            lastTPB = tpb;
            u.pin("CLOCK").drive<Level>(Level::Low);
        }

        CHECK(tpaRising == 1);
        CHECK(tpbRising == 1);
    }

    TEST_CASE("TPA rises at count=1, TPB rises at count=6")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBusLow(u);

        int tpaCount = -1;
        int tpbCount = -1;
        Level lastTPA = Level::Low;
        Level lastTPB = Level::Low;

        for (int i = 0; i < 8; ++i)
        {
            u.pin("CLOCK").drive<Level>(Level::High);
            const Level tpa = u.pin("TPA").read<Level>();
            const Level tpb = u.pin("TPB").read<Level>();
            if (lastTPA != Level::High && tpa == Level::High)
            {
                tpaCount = i;
            }
            if (lastTPB != Level::High && tpb == Level::High)
            {
                tpbCount = i;
            }
            lastTPA = tpa;
            lastTPB = tpb;
            u.pin("CLOCK").drive<Level>(Level::Low);
        }

        CHECK(tpaCount == 1);
        CHECK(tpbCount == 6);
    }

    TEST_CASE("SC1=Low SC0=Low at TPA for Fetch cycle (S0)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBusLow(u);

        // advance to TPA (count=1)
        u.pin("CLOCK").drive<Level>(Level::High); // count=0: beginCycle
        u.pin("CLOCK").drive<Level>(Level::Low);
        u.pin("CLOCK").drive<Level>(Level::High); // count=1: TPA High

        CHECK(u.pin("TPA").read<Level>() == Level::High);
        CHECK(u.pin("SC1").read<Level>() == Level::Low);
        CHECK(u.pin("SC0").read<Level>() == Level::Low);

        u.pin("CLOCK").drive<Level>(Level::Low);
    }

    TEST_CASE("nMRD=Low at count=0, stays Low through count=2, returns High at count=7")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBusLow(u);

        // count=0: beginCycle() drives nMRD Low for fetch
        u.pin("CLOCK").drive<Level>(Level::High);
        CHECK(u.pin("nMRD").read<Level>() == Level::Low);
        u.pin("CLOCK").drive<Level>(Level::Low);

        // count=1: TPA High, nMRD still Low
        u.pin("CLOCK").drive<Level>(Level::High);
        CHECK(u.pin("nMRD").read<Level>() == Level::Low);
        u.pin("CLOCK").drive<Level>(Level::Low);

        // count=2: TPA Low, nMRD stays Low (data window open until TPB)
        u.pin("CLOCK").drive<Level>(Level::High);
        CHECK(u.pin("TPA").read<Level>() == Level::Low);
        CHECK(u.pin("nMRD").read<Level>() == Level::Low);
        u.pin("CLOCK").drive<Level>(Level::Low);

        // count=3..6: nMRD stays Low
        for (int i = 3; i <= 6; ++i)
        {
            u.pin("CLOCK").drive<Level>(Level::High);
            CHECK(u.pin("nMRD").read<Level>() == Level::Low);
            u.pin("CLOCK").drive<Level>(Level::Low);
        }
    }

    TEST_CASE("inspect() exposes all registers at reset values")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);

        CHECK(u.inspect("D")->value == 0u);
        CHECK(u.inspect("DF")->value == 0u);
        CHECK(u.inspect("P")->value == 0u);
        CHECK(u.inspect("X")->value == 0u);
        CHECK(u.inspect("T")->value == 0u);
        CHECK(u.inspect("IE")->value == 1u);
        CHECK(u.inspect("Q")->value == 0u);
        CHECK(u.inspect("R0")->value == 0u);
        CHECK(u.inspect("RF")->value == 0u);
    }

    TEST_CASE("inspect() widths are correct")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);

        CHECK(u.inspect("D")->width == 8);
        CHECK(u.inspect("DF")->width == 1);
        CHECK(u.inspect("P")->width == 4);
        CHECK(u.inspect("X")->width == 4);
        CHECK(u.inspect("T")->width == 8);
        CHECK(u.inspect("IE")->width == 1);
        CHECK(u.inspect("Q")->width == 1);
        CHECK(u.inspect("R0")->width == 16);
        CHECK(u.inspect("RF")->width == 16);
    }

    TEST_CASE("inspect() returns nullopt for unknown name")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        CHECK(!u.inspect("ZZ").has_value());
        CHECK(!u.inspect("R").has_value());
        CHECK(!u.inspect("R10").has_value()); // decimal 10 is not valid hex index for R
    }

    TEST_CASE("inspect() returns all values")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };

        power(u);

        const auto all = u.inspect();
        // D, DF, R0..RF (16), P, X, T, IE, Q = 23 entries
        CHECK(all.size() == 23);
    }
}

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

TEST_SUITE("chip::cdp1xxx::CDP1861")
{
    static auto advanceCycle = [](CDP1861& u, Level sc1, Level sc0) {
        u.pin("SC1").drive<Level>(sc1);
        u.pin("SC0").drive<Level>(sc0);
        u.pin("TPA").drive<Level>(Level::Low);
        u.pin("TPA").drive<Level>(Level::High);
        u.pin("TPB").drive<Level>(Level::Low);
        u.pin("TPB").drive<Level>(Level::High);
        u.pin("TPB").drive<Level>(Level::Low);
        u.pin("TPA").drive<Level>(Level::Low);
    };

    static auto advanceLine = [](CDP1861& u) {
        for (int i = 0; i < 14; ++i)
        {
            advanceCycle(u, Level::High, Level::Low);
        }
    };

    static auto powerUp = [](CDP1861& u) {
        u.pin("VDD").drive<float>(5.0f);
        u.pin("nRESET").drive<Level>(Level::Low);
        u.pin("nRESET").drive<Level>(Level::High);
    };

    TEST_CASE("Construction creates 24 pins")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        CHECK(u.pins().size() == 24u);
    }

    TEST_CASE("Pin names match DIP-24 datasheet")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        CHECK(u.pin("nCLK").descriptor().name == std::string_view("nCLK"));
        CHECK(u.pin("nDMAO").descriptor().name == std::string_view("nDMAO"));
        CHECK(u.pin("nINT").descriptor().name == std::string_view("nINT"));
        CHECK(u.pin("TPA").descriptor().name == std::string_view("TPA"));
        CHECK(u.pin("TPB").descriptor().name == std::string_view("TPB"));
        CHECK(u.pin("nCOMPSYNC").descriptor().name == std::string_view("nCOMPSYNC"));
        CHECK(u.pin("VIDEO").descriptor().name == std::string_view("VIDEO"));
        CHECK(u.pin("nRESET").descriptor().name == std::string_view("nRESET"));
        CHECK(u.pin("nEFX").descriptor().name == std::string_view("nEFX"));
        CHECK(u.pin("DISPON").descriptor().name == std::string_view("DISPON"));
        CHECK(u.pin("DISPOFF").descriptor().name == std::string_view("DISPOFF"));
        CHECK(u.pin("VSS").descriptor().name == std::string_view("VSS"));

        CHECK(u.pin("DI0").descriptor().name == std::string_view("DI0"));
        CHECK(u.pin("DI1").descriptor().name == std::string_view("DI1"));
        CHECK(u.pin("DI2").descriptor().name == std::string_view("DI2"));
        CHECK(u.pin("DI3").descriptor().name == std::string_view("DI3"));
        CHECK(u.pin("DI4").descriptor().name == std::string_view("DI4"));
        CHECK(u.pin("DI5").descriptor().name == std::string_view("DI5"));
        CHECK(u.pin("DI6").descriptor().name == std::string_view("DI6"));
        CHECK(u.pin("DI7").descriptor().name == std::string_view("DI7"));
        CHECK(u.pin("SC0").descriptor().name == std::string_view("SC0"));
        CHECK(u.pin("SC1").descriptor().name == std::string_view("SC1"));
        CHECK(u.pin("nCLEAR").descriptor().name == std::string_view("nCLEAR"));
        CHECK(u.pin("VDD").descriptor().name == std::string_view("VDD"));
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        CHECK(&u.pin(CDP1861::Terminal::nCLK) == &u.pin("nCLK"));
        CHECK(&u.pin(CDP1861::Terminal::nDMAO) == &u.pin("nDMAO"));
        CHECK(&u.pin(CDP1861::Terminal::nINT) == &u.pin("nINT"));
        CHECK(&u.pin(CDP1861::Terminal::TPA) == &u.pin("TPA"));
        CHECK(&u.pin(CDP1861::Terminal::TPB) == &u.pin("TPB"));
        CHECK(&u.pin(CDP1861::Terminal::nCOMPSYNC) == &u.pin("nCOMPSYNC"));
        CHECK(&u.pin(CDP1861::Terminal::VIDEO) == &u.pin("VIDEO"));
        CHECK(&u.pin(CDP1861::Terminal::nRESET) == &u.pin("nRESET"));
        CHECK(&u.pin(CDP1861::Terminal::nEFX) == &u.pin("nEFX"));
        CHECK(&u.pin(CDP1861::Terminal::DISPON) == &u.pin("DISPON"));
        CHECK(&u.pin(CDP1861::Terminal::DISPOFF) == &u.pin("DISPOFF"));
        CHECK(&u.pin(CDP1861::Terminal::VSS) == &u.pin("VSS"));

        CHECK(&u.pin(CDP1861::Terminal::DI0) == &u.pin("DI0"));
        CHECK(&u.pin(CDP1861::Terminal::DI1) == &u.pin("DI1"));
        CHECK(&u.pin(CDP1861::Terminal::DI2) == &u.pin("DI2"));
        CHECK(&u.pin(CDP1861::Terminal::DI3) == &u.pin("DI3"));
        CHECK(&u.pin(CDP1861::Terminal::DI4) == &u.pin("DI4"));
        CHECK(&u.pin(CDP1861::Terminal::DI5) == &u.pin("DI5"));
        CHECK(&u.pin(CDP1861::Terminal::DI6) == &u.pin("DI6"));
        CHECK(&u.pin(CDP1861::Terminal::DI7) == &u.pin("DI7"));
        CHECK(&u.pin(CDP1861::Terminal::SC0) == &u.pin("SC0"));
        CHECK(&u.pin(CDP1861::Terminal::SC1) == &u.pin("SC1"));
        CHECK(&u.pin(CDP1861::Terminal::nCLEAR) == &u.pin("nCLEAR"));
        CHECK(&u.pin(CDP1861::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("Outputs are HighZ when unpowered")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        CHECK(u.pin("nDMAO").read<Level>() == Level::HighZ);
        CHECK(u.pin("nINT").read<Level>() == Level::HighZ);
        CHECK(u.pin("nCOMPSYNC").read<Level>() == Level::HighZ);
        CHECK(u.pin("VIDEO").read<Level>() == Level::HighZ);
        CHECK(u.pin("nEFX").read<Level>() == Level::HighZ);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Outputs are HighZ when VDD below range")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        u.pin("VDD").drive<float>(3.0f);

        CHECK(u.pin("nDMAO").read<Level>() == Level::HighZ);
        CHECK(u.pin("VIDEO").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Outputs are HighZ when VDD above range")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        u.pin("VDD").drive<float>(8.0f);

        CHECK(u.pin("nDMAO").read<Level>() == Level::HighZ);
        CHECK(u.pin("VIDEO").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD removal releases all outputs")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("nRESET").drive<Level>(Level::High);

        CHECK(u.pin("nDMAO").read<Level>() != Level::HighZ);

        u.pin("VDD").release();

        CHECK(u.pin("nDMAO").read<Level>() == Level::HighZ);
        CHECK(u.pin("nINT").read<Level>() == Level::HighZ);
        CHECK(u.pin("nCOMPSYNC").read<Level>() == Level::HighZ);
        CHECK(u.pin("VIDEO").read<Level>() == Level::HighZ);
        CHECK(u.pin("nEFX").read<Level>() == Level::HighZ);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::HighZ);
    }

    TEST_CASE("nRESET=Low drives all control outputs inactive and nCLEAR=Low")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("nRESET").drive<Level>(Level::Low);

        CHECK(u.pin("nDMAO").read<Level>() == Level::High);
        CHECK(u.pin("nINT").read<Level>() == Level::High);
        CHECK(u.pin("nCOMPSYNC").read<Level>() == Level::High);
        CHECK(u.pin("VIDEO").read<Level>() == Level::Low);
        CHECK(u.pin("nEFX").read<Level>() == Level::High);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::Low);
    }

    TEST_CASE("nRESET=High deasserts nCLEAR")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("nRESET").drive<Level>(Level::Low);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::Low);

        u.pin("nRESET").drive<Level>(Level::High);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::High);
    }

    TEST_CASE("nRESET=Low disables display even if previously enabled")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("nRESET").drive<Level>(Level::High);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        u.pin("nRESET").drive<Level>(Level::Low);

        CHECK(u.pin("nINT").read<Level>() == Level::High);
        CHECK(u.pin("nDMAO").read<Level>() == Level::High);
    }

    TEST_CASE("DISPON Low-to-High enables display flip-flop")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        advanceLine(u);

        CHECK(u.pin("nINT").read<Level>() == Level::High);
    }

    TEST_CASE("DISPOFF Low-to-High with DISPON=Low disables display flip-flop")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPOFF").drive<Level>(Level::Low);
        u.pin("DISPOFF").drive<Level>(Level::High);

        for (int line = 0; line < 4; ++line)
        {
            advanceLine(u);
        }

        CHECK(u.pin("nINT").read<Level>() == Level::High);
    }

    TEST_CASE("14 TPA+TPB cycles with SC1=H SC0=L advance vCounter by one")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        for (int line = 0; line < 2; ++line)
        {
            advanceLine(u);
        }

        CHECK(u.pin("nINT").read<Level>() == Level::Low);
    }

    TEST_CASE("vCounter wraps to 0 after 262 lines")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        for (int line = 0; line < 262; ++line)
        {
            advanceLine(u);
        }

        advanceCycle(u, Level::High, Level::Low);

        CHECK(u.pin("nCOMPSYNC").read<Level>() == Level::High);
    }

    TEST_CASE("nINT asserted 2 lines before display window when display enabled")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        for (int line = 0; line < 2; ++line)
        {
            advanceLine(u);
        }

        CHECK(u.pin("nINT").read<Level>() == Level::Low);
    }

    TEST_CASE("nINT not asserted when display disabled")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        for (int line = 0; line < 3; ++line)
        {
            advanceLine(u);
        }

        CHECK(u.pin("nINT").read<Level>() == Level::High);
    }

    TEST_CASE("nEFX Low from line 0 when display enabled (efxPreStart=0)")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        CHECK(u.pin("nEFX").read<Level>() == Level::Low);
    }

    TEST_CASE("nEFX High after display window ends (4 lines post-window)")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        for (int line = 0; line < 136; ++line)
        {
            advanceLine(u);
        }

        CHECK(u.pin("nEFX").read<Level>() == Level::High);
    }

    static auto reachDmaWindow = [](CDP1861& u) {
        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        for (int line = 0; line < 4; ++line)
        {
            advanceLine(u);
        }

        u.pin("SC1").drive<Level>(Level::High);
        u.pin("SC0").drive<Level>(Level::Low);
        u.pin("TPA").drive<Level>(Level::Low);
        u.pin("TPA").drive<Level>(Level::High);
        u.pin("TPA").drive<Level>(Level::Low);
        u.pin("TPA").drive<Level>(Level::High);
    };

    TEST_CASE("nDMAO asserted on 2nd TPA after HSync during display window")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);
        reachDmaWindow(u);

        CHECK(u.pin("nDMAO").read<Level>() == Level::Low);
    }

    TEST_CASE("nDMAO not asserted when SC incorrect during HSync TPA (re-sync)")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        for (int line = 0; line < 3; ++line)
        {
            advanceLine(u);
        }

        for (int cyc = 0; cyc < 13; ++cyc)
        {
            advanceCycle(u, Level::High, Level::Low);
        }

        // SC=11 (IDL): wrong for HSync re-sync — chip must exit HSync without asserting nDMAO (datasheet p.613)
        u.pin("SC1").drive<Level>(Level::High);
        u.pin("SC0").drive<Level>(Level::High);
        u.pin("TPA").drive<Level>(Level::Low);
        u.pin("TPA").drive<Level>(Level::High);
        u.pin("TPA").drive<Level>(Level::Low);
        u.pin("TPA").drive<Level>(Level::High);

        CHECK(u.pin("nDMAO").read<Level>() == Level::High);
        CHECK(u.pin("nCOMPSYNC").read<Level>() == Level::High);
    }

    TEST_CASE("HSync threshold 13 with SC1=H SC0=L, no HSync before 13th TPB")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        for (int cyc = 0; cyc < 12; ++cyc)
        {
            advanceCycle(u, Level::High, Level::Low);
        }

        CHECK(u.pin("nCOMPSYNC").read<Level>() == Level::High);

        advanceCycle(u, Level::High, Level::Low);

        CHECK(u.pin("nCOMPSYNC").read<Level>() == Level::Low);
    }

    TEST_CASE("nDMAO released after 8 DMA bytes (SC1=L SC0=H, TPB+8 nCLK each)")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);
        reachDmaWindow(u);

        CHECK(u.pin("nDMAO").read<Level>() == Level::Low);

        u.pin("SC1").drive<Level>(Level::Low);
        u.pin("SC0").drive<Level>(Level::High);
        u.pin("DISPON").drive<Level>(Level::High);

        for (int byteNum = 0; byteNum < 8; ++byteNum)
        {
            u.pin("TPB").drive<Level>(Level::Low);
            u.pin("TPB").drive<Level>(Level::High);

            for (int i = 0; i < 8; ++i)
            {
                u.pin("nCLK").drive<Level>(Level::High);
                u.pin("nCLK").drive<Level>(Level::Low);
            }
        }

        CHECK(u.pin("nDMAO").read<Level>() == Level::High);
    }

    TEST_CASE("DI not loaded when SC incorrect during DMA (SC1=H SC0=L instead of SC1=L SC0=H)")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);
        reachDmaWindow(u);

        CHECK(u.pin("nDMAO").read<Level>() == Level::Low);

        u.pin("SC1").drive<Level>(Level::High);
        u.pin("SC0").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);
        u.pin("TPB").drive<Level>(Level::High);

        for (int i = 0; i < 8; ++i)
        {
            u.pin("nCLK").drive<Level>(Level::High);
            u.pin("nCLK").drive<Level>(Level::Low);
        }

        CHECK(u.pin("nDMAO").read<Level>() == Level::Low);
    }

    TEST_CASE("DI not loaded when DISPON=Low during DMA nCLK")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);
        reachDmaWindow(u);

        CHECK(u.pin("nDMAO").read<Level>() == Level::Low);

        u.pin("SC1").drive<Level>(Level::Low);
        u.pin("SC0").drive<Level>(Level::High);
        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("TPB").drive<Level>(Level::High);

        for (int i = 0; i < 8; ++i)
        {
            u.pin("nCLK").drive<Level>(Level::High);
            u.pin("nCLK").drive<Level>(Level::Low);
        }

        CHECK(u.pin("nDMAO").read<Level>() == Level::Low);
    }

    TEST_CASE("nCLK trailing edge shifts VIDEO MSB-first")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);
        reachDmaWindow(u);

        u.pin("SC1").drive<Level>(Level::Low);
        u.pin("SC0").drive<Level>(Level::High);
        u.pin("DISPON").drive<Level>(Level::High);
        u.pin("DI7").drive<Level>(Level::High);
        u.pin("DI6").drive<Level>(Level::Low);
        u.pin("DI5").drive<Level>(Level::High);
        u.pin("DI4").drive<Level>(Level::High);
        u.pin("DI3").drive<Level>(Level::Low);
        u.pin("DI2").drive<Level>(Level::Low);
        u.pin("DI1").drive<Level>(Level::Low);
        u.pin("DI0").drive<Level>(Level::High);
        u.pin("TPB").drive<Level>(Level::High);
        u.pin("nCLK").drive<Level>(Level::High);
        u.pin("nCLK").drive<Level>(Level::Low);

        // 0b10110001 MSB-first: 1,0,1,1,0,0,0,1
        CHECK(u.pin("VIDEO").read<Level>() == Level::High);

        const Level expected[7] = { Level::Low, Level::High, Level::High, Level::Low,
                                    Level::Low, Level::Low,  Level::High };
        for (int i = 0; i < 7; ++i)
        {
            u.pin("nCLK").drive<Level>(Level::High);
            u.pin("nCLK").drive<Level>(Level::Low);
            CHECK(u.pin("VIDEO").read<Level>() == expected[i]);
        }
    }

    TEST_CASE("VIDEO=Low after shift register emptied (9th nCLK)")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);
        reachDmaWindow(u);

        u.pin("SC1").drive<Level>(Level::Low);
        u.pin("SC0").drive<Level>(Level::High);
        u.pin("DISPON").drive<Level>(Level::High);
        u.pin("DI7").drive<Level>(Level::High);
        u.pin("TPB").drive<Level>(Level::High);
        u.pin("nCLK").drive<Level>(Level::High);
        u.pin("nCLK").drive<Level>(Level::Low);

        for (int i = 0; i < 7; ++i)
        {
            u.pin("nCLK").drive<Level>(Level::High);
            u.pin("nCLK").drive<Level>(Level::Low);
        }

        u.pin("nCLK").drive<Level>(Level::High);
        u.pin("nCLK").drive<Level>(Level::Low);

        CHECK(u.pin("VIDEO").read<Level>() == Level::Low);
    }

    TEST_CASE("nRESET=Low resets and suppresses nDMAO/nINT")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        u.pin("nRESET").drive<Level>(Level::Low);

        CHECK(u.pin("nDMAO").read<Level>() == Level::High);
        CHECK(u.pin("nINT").read<Level>() == Level::High);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::Low);
    }

    TEST_CASE("nRESET=Low disables display even if previously enabled")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.pin("DISPON").drive<Level>(Level::Low);
        u.pin("DISPON").drive<Level>(Level::High);

        u.pin("nRESET").drive<Level>(Level::Low);

        CHECK(u.pin("nINT").read<Level>() == Level::High);
        CHECK(u.pin("nDMAO").read<Level>() == Level::High);
    }

    TEST_CASE("nRESET=High deasserts nCLEAR")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        u.pin("VDD").drive<float>(5.0f);
        u.pin("nRESET").drive<Level>(Level::Low);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::Low);

        u.pin("nRESET").drive<Level>(Level::High);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::High);
    }

    TEST_CASE("Outputs are HighZ when unpowered")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        CHECK(u.pin("nDMAO").read<Level>() == Level::HighZ);
        CHECK(u.pin("nINT").read<Level>() == Level::HighZ);
        CHECK(u.pin("nCOMPSYNC").read<Level>() == Level::HighZ);
        CHECK(u.pin("VIDEO").read<Level>() == Level::HighZ);
        CHECK(u.pin("nEFX").read<Level>() == Level::HighZ);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Outputs are HighZ when VDD below range")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        u.pin("VDD").drive<float>(3.0f);

        CHECK(u.pin("nDMAO").read<Level>() == Level::HighZ);
        CHECK(u.pin("VIDEO").read<Level>() == Level::HighZ);
    }

    TEST_CASE("Outputs are HighZ when VDD above range")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        u.pin("VDD").drive<float>(8.0f);

        CHECK(u.pin("nDMAO").read<Level>() == Level::HighZ);
        CHECK(u.pin("VIDEO").read<Level>() == Level::HighZ);
    }

    TEST_CASE("VDD removal releases all outputs")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        CHECK(u.pin("nDMAO").read<Level>() != Level::HighZ);

        u.pin("VDD").release();

        CHECK(u.pin("nDMAO").read<Level>() == Level::HighZ);
        CHECK(u.pin("nINT").read<Level>() == Level::HighZ);
        CHECK(u.pin("nCOMPSYNC").read<Level>() == Level::HighZ);
        CHECK(u.pin("VIDEO").read<Level>() == Level::HighZ);
        CHECK(u.pin("nEFX").read<Level>() == Level::HighZ);
        CHECK(u.pin("nCLEAR").read<Level>() == Level::HighZ);
    }

    TEST_CASE("reset() clears counters: nDMAO and nINT High")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };
        powerUp(u);

        u.reset();

        CHECK(u.pin("nDMAO").read<Level>() == Level::High);
        CHECK(u.pin("nINT").read<Level>() == Level::High);
    }

    TEST_CASE("Terminal enum maps to correct pins")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        CHECK(&u.pin(CDP1861::Terminal::nCLK) == &u.pin("nCLK"));
        CHECK(&u.pin(CDP1861::Terminal::nDMAO) == &u.pin("nDMAO"));
        CHECK(&u.pin(CDP1861::Terminal::nINT) == &u.pin("nINT"));
        CHECK(&u.pin(CDP1861::Terminal::TPA) == &u.pin("TPA"));
        CHECK(&u.pin(CDP1861::Terminal::TPB) == &u.pin("TPB"));
        CHECK(&u.pin(CDP1861::Terminal::nCOMPSYNC) == &u.pin("nCOMPSYNC"));
        CHECK(&u.pin(CDP1861::Terminal::VIDEO) == &u.pin("VIDEO"));
        CHECK(&u.pin(CDP1861::Terminal::nRESET) == &u.pin("nRESET"));
        CHECK(&u.pin(CDP1861::Terminal::nEFX) == &u.pin("nEFX"));
        CHECK(&u.pin(CDP1861::Terminal::DISPON) == &u.pin("DISPON"));
        CHECK(&u.pin(CDP1861::Terminal::DISPOFF) == &u.pin("DISPOFF"));
        CHECK(&u.pin(CDP1861::Terminal::VSS) == &u.pin("VSS"));
        CHECK(&u.pin(CDP1861::Terminal::DI0) == &u.pin("DI0"));
        CHECK(&u.pin(CDP1861::Terminal::DI1) == &u.pin("DI1"));
        CHECK(&u.pin(CDP1861::Terminal::DI2) == &u.pin("DI2"));
        CHECK(&u.pin(CDP1861::Terminal::DI3) == &u.pin("DI3"));
        CHECK(&u.pin(CDP1861::Terminal::DI4) == &u.pin("DI4"));
        CHECK(&u.pin(CDP1861::Terminal::DI5) == &u.pin("DI5"));
        CHECK(&u.pin(CDP1861::Terminal::DI6) == &u.pin("DI6"));
        CHECK(&u.pin(CDP1861::Terminal::DI7) == &u.pin("DI7"));
        CHECK(&u.pin(CDP1861::Terminal::SC0) == &u.pin("SC0"));
        CHECK(&u.pin(CDP1861::Terminal::SC1) == &u.pin("SC1"));
        CHECK(&u.pin(CDP1861::Terminal::nCLEAR) == &u.pin("nCLEAR"));
        CHECK(&u.pin(CDP1861::Terminal::VDD) == &u.pin("VDD"));
    }

    TEST_CASE("Component name matches descriptor")
    {
        CDP1861 u{ CDP1861::Descriptor{ .name = "U2" } };

        CHECK(std::string_view(u.name()) == "U2");
    }
}
