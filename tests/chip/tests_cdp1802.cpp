#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::chip::cdp1xxx;
using namespace nfx::silicon::signal;

namespace
{
    void power(CDP1802& u)
    {
        u.pin("VDD").drive<float>(5.0f);
    }

    void clockN(CDP1802& u, int n)
    {
        for (int i = 0; i < n; ++i)
        {
            u.pin("CLOCK").drive<Level>(Level::High);
            u.pin("CLOCK").drive<Level>(Level::Low);
        }
    }

    void driveBus(CDP1802& u, std::uint8_t byte)
    {
        static const char* k_bus[8] = { "BUS0", "BUS1", "BUS2", "BUS3", "BUS4", "BUS5", "BUS6", "BUS7" };
        for (int i = 0; i < 8; ++i)
        {
            u.pin(k_bus[i]).drive<Level>((byte & (1u << i)) ? Level::High : Level::Low);
        }
    }

    void runInstruction(CDP1802& u, std::uint8_t fetchByte, std::uint8_t execByte)
    {
        driveBus(u, fetchByte);
        clockN(u, 8);
        driveBus(u, execByte);
        clockN(u, 8);
    }
} // namespace

TEST_SUITE("chip::cdp1xxx::CDP1802 MemoryReference")
{
    TEST_CASE("LDI: M(R(P))->D; R(P)+1->R(P)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // LDI = 0xF8, operand = 0xAB
        runInstruction(u, 0xF8, 0xAB);

        CHECK(u.inspect("D")->value == 0xABu);
        CHECK(u.inspect("R0")->value == 0x0002u); // R(P=0) incremented twice: fetch + LDI operand
    }

    TEST_CASE("LDN: M(R(N))->D for N!=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0x01, 0xCD);

        CHECK(u.inspect("D")->value == 0xCDu);
        CHECK(u.inspect("R0")->value == 0x0001u); // R(P=0) incremented once during fetch
    }

    TEST_CASE("LDN: N=0 is IDL, D unchanged")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // Fetch: 0x00 = IDL/LDN(N=0). D must remain 0.
        runInstruction(u, 0x00, 0x42);

        CHECK(u.inspect("D")->value == 0x00u);
    }

    TEST_CASE("STR: nMWR pulses Low then High during Execute")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x51);
        clockN(u, 8); // Fetch

        // During Execute cycle, observe nMWR: drive bus low, run clocks one by one
        driveBus(u, 0x00);

        bool sawNMWRLow = false;
        for (int i = 0; i < 8; ++i)
        {
            u.pin("CLOCK").drive<Level>(Level::High);
            if (u.pin("nMWR").read<Level>() == Level::Low)
            {
                sawNMWRLow = true;
            }
            u.pin("CLOCK").drive<Level>(Level::Low);
        }

        u.pin("CLOCK").drive<Level>(Level::High); // tick 0 of next cycle: deasserts nMWR
        CHECK(sawNMWRLow);
        CHECK(u.pin("nMWR").read<Level>() == Level::High);
    }

    TEST_CASE("STXD: nMWR pulses during Execute; R(X) decremented")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // Fetch: 0x73 = STXD. X=0 at reset, so R(X)=R(0)=0x0001 after fetch R(P)++.
        driveBus(u, 0x73);
        clockN(u, 8); // Fetch: R(0) is now 0x0001

        driveBus(u, 0x00);

        bool sawNMWRLow = false;
        for (int i = 0; i < 8; ++i)
        {
            u.pin("CLOCK").drive<Level>(Level::High);
            if (u.pin("nMWR").read<Level>() == Level::Low)
            {
                sawNMWRLow = true;
            }
            u.pin("CLOCK").drive<Level>(Level::Low);
        }

        u.pin("CLOCK").drive<Level>(Level::High); // tick 0 of next cycle: deasserts nMWR
        CHECK(sawNMWRLow);
        CHECK(u.pin("nMWR").read<Level>() == Level::High);

        // R(X=0) was 0x0001 after fetch, STXD decrements it to 0x0000
        CHECK(u.inspect("R0")->value == 0x0000u);
    }
}

TEST_SUITE("chip::cdp1xxx::CDP1802 RegisterOperations")
{
    TEST_CASE("INC: R(N)+1->R(N)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0x11, 0x00); // INC R(1), R(1)=0x0000 at reset
        CHECK(u.inspect("R1")->value == 0x0001u);
    }

    TEST_CASE("DEC: R(N)-1->R(N) wraps at 0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0x21, 0x00); // DEC R(1), R(1)=0x0000 -> 0xFFFF
        CHECK(u.inspect("R1")->value == 0xFFFFu);
    }

    TEST_CASE("IRX: R(X)+1->R(X)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // X=0 at reset, after fetch R(0)=0x0001, IRX increments to 0x0002
        runInstruction(u, 0x60, 0x00);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("GLO: R(N).lo->D")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0xCD); // LDI 0xCD -> D=0xCD
        runInstruction(u, 0xA1, 0x00); // PLO R(1) -> R(1).lo=0xCD
        runInstruction(u, 0x81, 0x00); // GLO R(1) -> D=0xCD
        CHECK(u.inspect("D")->value == 0xCDu);
    }

    TEST_CASE("GHI: R(N).hi->D")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0xAB); // LDI 0xAB -> D=0xAB
        runInstruction(u, 0xB1, 0x00); // PHI R(1) -> R(1).hi=0xAB
        runInstruction(u, 0x91, 0x00); // GHI R(1) -> D=0xAB
        CHECK(u.inspect("D")->value == 0xABu);
    }

    TEST_CASE("PLO: D->R(N).lo, hi unchanged")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0xAB); // LDI 0xAB
        runInstruction(u, 0xB1, 0x00); // PHI R(1) -> R(1)=0xAB00
        runInstruction(u, 0xF8, 0xCD); // LDI 0xCD
        runInstruction(u, 0xA1, 0x00); // PLO R(1) -> R(1)=0xABCD
        CHECK(u.inspect("R1")->value == 0xABCDu);
    }

    TEST_CASE("PHI: D->R(N).hi, lo unchanged")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0xCD); // LDI 0xCD
        runInstruction(u, 0xA1, 0x00); // PLO R(1) -> R(1)=0x00CD
        runInstruction(u, 0xF8, 0xAB); // LDI 0xAB
        runInstruction(u, 0xB1, 0x00); // PHI R(1) -> R(1)=0xABCD
        CHECK(u.inspect("R1")->value == 0xABCDu);
    }
}
