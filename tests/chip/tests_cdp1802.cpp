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

TEST_SUITE("chip::cdp1xxx::CDP1802 LogicOperations")
{
    TEST_CASE("OR: M(R(X)) OR D->D")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x0F); // LDI 0x0F -> D=0x0F
        driveBus(u, 0xF1);             // fetch OR
        clockN(u, 8);
        driveBus(u, 0xF0); // M(R(X=0))=0xF0 during execute
        clockN(u, 8);
        CHECK(u.inspect("D")->value == 0xFFu);
    }

    TEST_CASE("ORI: M(R(P)) OR D->D, R(P)+1->R(P)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x0F); // LDI 0x0F -> D=0x0F
        runInstruction(u, 0xF9, 0xF0); // ORI 0xF0 -> D=0xFF
        CHECK(u.inspect("D")->value == 0xFFu);
    }

    TEST_CASE("XOR: M(R(X)) XOR D->D")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0xFF); // LDI 0xFF -> D=0xFF
        driveBus(u, 0xF3);             // fetch XOR
        clockN(u, 8);
        driveBus(u, 0xFF); // M(R(X=0))=0xFF during execute
        clockN(u, 8);
        CHECK(u.inspect("D")->value == 0x00u);
    }

    TEST_CASE("XRI: M(R(P)) XOR D->D, R(P)+1->R(P)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0xFF); // LDI 0xFF -> D=0xFF
        runInstruction(u, 0xFB, 0xFF); // XRI 0xFF -> D=0x00
        CHECK(u.inspect("D")->value == 0x00u);
    }

    TEST_CASE("AND: M(R(X)) AND D->D")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0xFF); // LDI 0xFF -> D=0xFF
        driveBus(u, 0xF2);             // fetch AND
        clockN(u, 8);
        driveBus(u, 0x0F); // M(R(X=0))=0x0F during execute
        clockN(u, 8);
        CHECK(u.inspect("D")->value == 0x0Fu);
    }

    TEST_CASE("ANI: M(R(P)) AND D->D, R(P)+1->R(P)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0xFF); // LDI 0xFF -> D=0xFF
        runInstruction(u, 0xFA, 0x0F); // ANI 0x0F -> D=0x0F
        CHECK(u.inspect("D")->value == 0x0Fu);
    }

    TEST_CASE("SHR: D>>1->D, D.0->DF, 0->D.7")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x81); // LDI 0x81 -> D=0x81
        runInstruction(u, 0xF6, 0x00); // SHR -> D=0x40, DF=1
        CHECK(u.inspect("D")->value == 0x40u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SHRC: D>>1->D, D.0->DF, DF->D.7")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x81); // LDI 0x81 -> D=0x81
        runInstruction(u, 0xF6, 0x00); // SHR -> D=0x40, DF=1
        runInstruction(u, 0x76, 0x00); // SHRC: D=0x40, DF=1 -> D=0xA0, DF=0
        CHECK(u.inspect("D")->value == 0xA0u);
        CHECK(u.inspect("DF")->value == 0u);
    }

    TEST_CASE("SHL: D<<1->D, D.7->DF, 0->D.0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x81); // LDI 0x81 -> D=0x81
        runInstruction(u, 0xFE, 0x00); // SHL -> D=0x02, DF=1
        CHECK(u.inspect("D")->value == 0x02u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SHLC: D<<1->D, D.7->DF, DF->D.0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x81); // LDI 0x81 -> D=0x81
        runInstruction(u, 0xFE, 0x00); // SHL -> D=0x02, DF=1
        runInstruction(u, 0x7E, 0x00); // SHLC: D=0x02, DF=1 -> D=0x05, DF=0
        CHECK(u.inspect("D")->value == 0x05u);
        CHECK(u.inspect("DF")->value == 0u);
    }
}

TEST_SUITE("chip::cdp1xxx::CDP1802 ArithmeticOperations")
{
    TEST_CASE("ADD: M(R(X))+D->DF,D")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0xF0); // LDI 0xF0 -> D=0xF0
        driveBus(u, 0xF4);             // fetch ADD
        clockN(u, 8);
        driveBus(u, 0x20); // M(R(X=0))=0x20 during execute
        clockN(u, 8);
        CHECK(u.inspect("D")->value == 0x10u);
        CHECK(u.inspect("DF")->value == 1u); // carry
    }

    TEST_CASE("ADI: M(R(P))+D->DF,D, R(P)+1->R(P)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF -> D=0x00, DF=1
        CHECK(u.inspect("D")->value == 0x00u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("ADC: M(R(X))+D+DF->DF,D")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF -> D=0x00, DF=1
        driveBus(u, 0x74);             // fetch ADC
        clockN(u, 8);
        driveBus(u, 0x01); // M(R(X=0))=0x01, D=0x00, DF=1 -> D=0x02, DF=0
        clockN(u, 8);
        CHECK(u.inspect("D")->value == 0x02u);
        CHECK(u.inspect("DF")->value == 0u);
    }

    TEST_CASE("ADI: no carry when result fits in 8 bits")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x10); // LDI 0x10
        runInstruction(u, 0xFC, 0x20); // ADI 0x20 -> D=0x30, DF=0
        CHECK(u.inspect("D")->value == 0x30u);
        CHECK(u.inspect("DF")->value == 0u);
    }

    TEST_CASE("SD: M(R(X))-D->DF,D, DF=1 if no borrow")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        driveBus(u, 0xF5);             // fetch SD
        clockN(u, 8);
        driveBus(u, 0x03); // M(R(X=0))=0x03, 0x03-0x01=0x02, no borrow -> DF=1
        clockN(u, 8);
        CHECK(u.inspect("D")->value == 0x02u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SD: borrow sets DF=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x05); // LDI 0x05 -> D=0x05
        driveBus(u, 0xF5);             // fetch SD
        clockN(u, 8);
        driveBus(u, 0x02); // M(R(X=0))=0x02, 0x02-0x05 -> borrow -> DF=0
        clockN(u, 8);
        CHECK(u.inspect("DF")->value == 0u);
    }

    TEST_CASE("SDI: M(R(P))-D->DF,D, R(P)+1->R(P)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFD, 0x05); // SDI 0x05, 0x05-0x01=0x04, no borrow -> DF=1
        CHECK(u.inspect("D")->value == 0x04u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SM: D-M(R(X))->DF,D, DF=1 if no borrow")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x05); // LDI 0x05 -> D=0x05
        driveBus(u, 0xF7);             // fetch SM
        clockN(u, 8);
        driveBus(u, 0x03); // M(R(X=0))=0x03, 0x05-0x03=0x02, no borrow -> DF=1
        clockN(u, 8);
        CHECK(u.inspect("D")->value == 0x02u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SMI: D-M(R(P))->DF,D, R(P)+1->R(P)")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x05); // LDI 0x05 -> D=0x05
        runInstruction(u, 0xFF, 0x03); // SMI 0x03, 0x05-0x03=0x02, no borrow -> DF=1
        CHECK(u.inspect("D")->value == 0x02u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("ADCI: M(R(P))+D+DF->DF,D, carry-in=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // DF=0 after reset
        runInstruction(u, 0xF8, 0x10); // LDI 0x10 -> D=0x10
        runInstruction(u, 0x7C, 0x05); // ADCI 0x05, 0x10+0x05+0=0x15, no carry -> DF=0
        CHECK(u.inspect("D")->value == 0x15u);
        CHECK(u.inspect("DF")->value == 0u);
    }

    TEST_CASE("ADCI: M(R(P))+D+DF->DF,D, carry-in=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF, 0x01+0xFF=0x100 -> DF=1
        runInstruction(u, 0x7C, 0x05); // ADCI 0x05, 0x00+0x05+1=0x06, no carry -> DF=0
        CHECK(u.inspect("D")->value == 0x06u);
        CHECK(u.inspect("DF")->value == 0u);
    }

    TEST_CASE("SDB: M(R(X))-D-(NOT DF)->DF,D, no borrow-in")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // DF=0 after reset, NOT DF=1
        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        driveBus(u, 0x75);             // fetch SDB
        clockN(u, 8);
        driveBus(u, 0x05); // M(R(X=0))=0x05, 0x05-0x01-1=0x03, no borrow -> DF=1
        clockN(u, 8);
        CHECK(u.inspect("D")->value == 0x03u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SDB: M(R(X))-D-(NOT DF)->DF,D, borrow")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // DF=0 after reset, NOT DF=1
        runInstruction(u, 0xF8, 0x05); // LDI 0x05 -> D=0x05
        driveBus(u, 0x75);             // fetch SDB
        clockN(u, 8);
        driveBus(u, 0x02); // M(R(X=0))=0x02, 0x02-0x05-1 -> borrow -> DF=0
        clockN(u, 8);
        CHECK(u.inspect("DF")->value == 0u);
    }

    TEST_CASE("SDBI: M(R(P))-D-(NOT DF)->DF,D, no borrow-in")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // DF=0 after reset, NOT DF=1
        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0x7D, 0x05); // SDBI 0x05, 0x05-0x01-1=0x03, no borrow -> DF=1
        CHECK(u.inspect("D")->value == 0x03u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SDBI: M(R(P))-D-(NOT DF)->DF,D, borrow-in")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF -> DF=1 (carry out), NOT DF=0
        runInstruction(u, 0x7D, 0x05); // SDBI 0x05, 0x00+0x05-0=0x05, no borrow -> DF=1
        CHECK(u.inspect("D")->value == 0x05u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SMB: D-M(R(X))-(NOT DF)->DF,D, no borrow-in")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // DF=0 after reset, NOT DF=1
        runInstruction(u, 0xF8, 0x05); // LDI 0x05 -> D=0x05
        driveBus(u, 0x77);             // fetch SMB
        clockN(u, 8);
        driveBus(u, 0x02); // M(R(X=0))=0x02, 0x05-0x02-1=0x02, no borrow -> DF=1
        clockN(u, 8);
        CHECK(u.inspect("D")->value == 0x02u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SMB: D-M(R(X))-(NOT DF)->DF,D, borrow")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // DF=0 after reset, NOT DF=1
        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        driveBus(u, 0x77);             // fetch SMB
        clockN(u, 8);
        driveBus(u, 0x05); // M(R(X=0))=0x05, 0x01-0x05-1 -> borrow -> DF=0
        clockN(u, 8);
        CHECK(u.inspect("DF")->value == 0u);
    }

    TEST_CASE("SMBI: D-M(R(P))-(NOT DF)->DF,D, no borrow-in")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // DF=0 after reset, NOT DF=1
        runInstruction(u, 0xF8, 0x05); // LDI 0x05 -> D=0x05
        runInstruction(u, 0x7F, 0x02); // SMBI 0x02, 0x05-0x02-1=0x02, no borrow -> DF=1
        CHECK(u.inspect("D")->value == 0x02u);
        CHECK(u.inspect("DF")->value == 1u);
    }

    TEST_CASE("SMBI: D-M(R(P))-(NOT DF)->DF,D, borrow-in")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF, 0x01+0xFF=0x100 -> D=0x00, DF=1, NOT DF=0
        runInstruction(u, 0x7F, 0x02); // SMBI 0x02, 0x00-0x02-0 -> borrow -> DF=0
        CHECK(u.inspect("DF")->value == 0u);
    }
}

TEST_SUITE("chip::cdp1xxx::CDP1802 BranchInstructions")
{
    TEST_CASE("BR: M(R(P))->R(P).0, unconditional")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // R(P)=0x0000, fetch BR at 0x0000, execute reads 0x42 from bus -> R(P)=0x0042
        driveBus(u, 0x30);
        clockN(u, 8);
        driveBus(u, 0x42);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0042u);
    }

    TEST_CASE("NBR/SKP: R(P)++, always skip branch byte")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // fetch 0x38: R(P) 0x0000->0x0001, execute R(P)++ -> 0x0002
        driveBus(u, 0x38);
        clockN(u, 8);
        driveBus(u, 0xAA); // on bus during execute, not fetched
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("BZ: branch taken when D=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // D=0 after reset, BZ must branch -> R(P).0=0x55
        driveBus(u, 0x32);
        clockN(u, 8);
        driveBus(u, 0x55);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0055u);
    }

    TEST_CASE("BNZ: branch not taken when D=0, R(P)++")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // D=0 after reset, BNZ condition false -> fetch reads branch byte into R(P) then ++
        // fetch 0x3A: R(P)->0x0001, execute: read(R(P)=0x0001)=0x55, no branch -> R(P)++ -> 0x0002
        driveBus(u, 0x3A);
        clockN(u, 8);
        driveBus(u, 0x55); // branch byte, skipped
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("BNF: branch taken when DF=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // DF=0 after reset, BNF must branch -> R(P).0=0x10
        driveBus(u, 0x3B);
        clockN(u, 8);
        driveBus(u, 0x10);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0010u);
    }

    TEST_CASE("BDF: branch taken when DF=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF -> D=0x00, DF=1
        driveBus(u, 0x33);
        clockN(u, 8);
        driveBus(u, 0x20); // branch target low byte, R(P).1 unchanged
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0020u);
    }

    TEST_CASE("B1: taken when EF1=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        u.pin("nEF1").drive<Level>(Level::Low); // EF1=1 (active low)

        driveBus(u, 0x34);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0020u);
    }

    TEST_CASE("B1: not taken when EF1=0, R(P)++")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        // nEF1 not driven: EF1=0

        driveBus(u, 0x34);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("B2: taken when EF2=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        u.pin("nEF2").drive<Level>(Level::Low);

        driveBus(u, 0x35);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0020u);
    }

    TEST_CASE("B2: not taken when EF2=0, R(P)++")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x35);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("B3: taken when EF3=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        u.pin("nEF3").drive<Level>(Level::Low);

        driveBus(u, 0x36);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0020u);
    }

    TEST_CASE("B3: not taken when EF3=0, R(P)++")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x36);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("B4: taken when EF4=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        u.pin("nEF4").drive<Level>(Level::Low);

        driveBus(u, 0x37);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0020u);
    }

    TEST_CASE("B4: not taken when EF4=0, R(P)++")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x37);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("BN1: taken when EF1=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        // nEF1 not driven: EF1=0

        driveBus(u, 0x3C);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0020u);
    }

    TEST_CASE("BN1: not taken when EF1=1, R(P)++")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        u.pin("nEF1").drive<Level>(Level::Low);

        driveBus(u, 0x3C);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("BN2: taken when EF2=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x3D);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0020u);
    }

    TEST_CASE("BN2: not taken when EF2=1, R(P)++")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        u.pin("nEF2").drive<Level>(Level::Low);

        driveBus(u, 0x3D);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("BN3: taken when EF3=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x3E);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0020u);
    }

    TEST_CASE("BN3: not taken when EF3=1, R(P)++")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        u.pin("nEF3").drive<Level>(Level::Low);

        driveBus(u, 0x3E);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("BN4: taken when EF4=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x3F);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0020u);
    }

    TEST_CASE("BN4: not taken when EF4=1, R(P)++")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);
        u.pin("nEF4").drive<Level>(Level::Low);

        driveBus(u, 0x3F);
        clockN(u, 8);
        driveBus(u, 0x20);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("NLBR/LSKP: R(P)+=2, always skip 2 bytes")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // fetch 0xC8: R(P)->0x0001, execute R(P)+=2->0x0003, extra cycle silent
        driveBus(u, 0xC8);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0003u);
    }

    TEST_CASE("LSZ: R(P)+=2 when D=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // D=0 after reset, fetch 0xCE: R(P)->0x0001, execute R(P)+=2->0x0003, extra cycle silent
        driveBus(u, 0xCE);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0003u);
    }

    TEST_CASE("LSNZ: no skip when D=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // D=0 after reset, condition false -> no skip, extra cycle silent
        driveBus(u, 0xC6);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0001u);
    }

    TEST_CASE("LSDF: R(P)+=2 when DF=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF -> DF=1
        const auto rp_before = u.inspect("R0")->value;
        driveBus(u, 0xCF);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(
            u.inspect("R0")->value ==
            static_cast<std::uint16_t>(rp_before + 3u)); // fetch +1, execute +2, extra cycle silent
    }

    TEST_CASE("LSNF: no skip when DF=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF -> DF=1
        const auto rp_before = u.inspect("R0")->value;
        driveBus(u, 0xC7); // LSNF, DF=1 -> condition false, no skip
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == static_cast<std::uint16_t>(rp_before + 1u)); // fetch +1, extra cycle silent
    }

    TEST_CASE("LBQ: not taken when Q=0, R(P)+=2")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0xC1);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0003u);
    }

    TEST_CASE("LBNQ: not taken when Q=1, R(P)+=2")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0x7B, 0x00); // SEQ -> Q=1
        const auto rp_before = u.inspect("R0")->value;
        driveBus(u, 0xC9);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == static_cast<std::uint16_t>(rp_before + 3u));
    }

    TEST_CASE("LBZ: not taken when D!=0, R(P)+=2")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        const auto rp_before = u.inspect("R0")->value;
        driveBus(u, 0xC2);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == static_cast<std::uint16_t>(rp_before + 3u));
    }

    TEST_CASE("LBNZ: not taken when D=0, R(P)+=2")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0xCA);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0003u);
    }

    TEST_CASE("LBDF: not taken when DF=0, R(P)+=2")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        const auto rp_before = u.inspect("R0")->value;
        driveBus(u, 0xC3);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == static_cast<std::uint16_t>(rp_before + 3u));
    }

    TEST_CASE("LBNF: not taken when DF=1, R(P)+=2")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF -> DF=1
        const auto rp_before = u.inspect("R0")->value;
        driveBus(u, 0xCB);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == static_cast<std::uint16_t>(rp_before + 3u));
    }

    TEST_CASE("LBR: branches to target address after 3 real machine cycles")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0xC0);
        clockN(u, 8);
        driveBus(u, 0x12);
        clockN(u, 8);
        driveBus(u, 0x34);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x1234u);
    }

    TEST_CASE("LBQ: taken when Q=1, branches to target address")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0x7B, 0x00); // SEQ -> Q=1
        driveBus(u, 0xC1);
        clockN(u, 8);
        driveBus(u, 0x12);
        clockN(u, 8);
        driveBus(u, 0x34);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x1234u);
    }

    TEST_CASE("LBNQ: taken when Q=0, branches to target address")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0xC9);
        clockN(u, 8);
        driveBus(u, 0x12);
        clockN(u, 8);
        driveBus(u, 0x34);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x1234u);
    }

    TEST_CASE("LBZ: taken when D=0, branches to target address")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0xC2);
        clockN(u, 8);
        driveBus(u, 0x12);
        clockN(u, 8);
        driveBus(u, 0x34);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x1234u);
    }

    TEST_CASE("LBNZ: taken when D!=0, branches to target address")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        driveBus(u, 0xCA);
        clockN(u, 8);
        driveBus(u, 0x12);
        clockN(u, 8);
        driveBus(u, 0x34);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x1234u);
    }

    TEST_CASE("LBDF: taken when DF=1, branches to target address")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xF8, 0x01); // LDI 0x01 -> D=0x01
        runInstruction(u, 0xFC, 0xFF); // ADI 0xFF -> DF=1
        driveBus(u, 0xC3);
        clockN(u, 8);
        driveBus(u, 0x12);
        clockN(u, 8);
        driveBus(u, 0x34);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x1234u);
    }

    TEST_CASE("LBNF: taken when DF=0, branches to target address")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0xCB);
        clockN(u, 8);
        driveBus(u, 0x12);
        clockN(u, 8);
        driveBus(u, 0x34);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x1234u);
    }

    TEST_CASE("executePhase resets between instructions")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // NLBR (3 cycles): executePhase goes 0 then 1 across two Execute cycles
        driveBus(u, 0xC8);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);

        // LDI 0x99: must dispatch at phase=0, D must be 0x99
        runInstruction(u, 0xF8, 0x99);
        CHECK(u.inspect("D")->value == 0x99u);
    }
}

TEST_SUITE("chip::cdp1xxx::CDP1802 ControlInstructions")
{
    TEST_CASE("IDL: opcode 0x00 dispatches to IDL not LDN, SC=S1 on next cycle")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x00); // IDL
        clockN(u, 8);      // Fetch: opcode 0x00
        driveBus(u, 0x42); // bus value LDN would read, must be ignored
        clockN(u, 8);      // Execute: IDL sets idling, D must not change
        CHECK(u.inspect("D")->value == 0x00u);

        // CPU stays in S1 (SC1=Low, SC0=High)
        u.pin("CLOCK").drive<Level>(Level::High);
        CHECK(u.pin("SC1").read<Level>() == Level::Low);
        CHECK(u.pin("SC0").read<Level>() == Level::High);
    }

    TEST_CASE("IDL: CPU suspended, R(P) does not advance until DMA or INT")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // Fetch+Execute IDL: R(0)=0x0001 after fetch, CPU enters idle
        driveBus(u, 0x00);
        clockN(u, 8);
        driveBus(u, 0x00);
        clockN(u, 8);
        const auto rp_after_idl = u.inspect("R0")->value;

        // Clock 3 more full machine cycles: CPU must stay idle, R(0) must not change
        clockN(u, 8);
        clockN(u, 8);
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == rp_after_idl);
    }

    TEST_CASE("SEP: N->P")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xD3, 0x00); // SEP R3 -> P=3
        CHECK(u.inspect("P")->value == 3u);
    }

    TEST_CASE("SEX: N->X")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0xE5, 0x00); // SEX R5 -> X=5
        CHECK(u.inspect("X")->value == 5u);
    }

    TEST_CASE("SEQ: 1->Q, Q pin High")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0x7B, 0x00);            // SEQ
        u.pin("CLOCK").drive<Level>(Level::High); // beginCycle of next cycle propagates Q pin
        CHECK(u.inspect("Q")->value == 1u);
        CHECK(u.pin("Q").read<Level>() == Level::High);
    }

    TEST_CASE("REQ: 0->Q, Q pin Low")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        runInstruction(u, 0x7B, 0x00);            // SEQ -> Q=1
        runInstruction(u, 0x7A, 0x00);            // REQ -> Q=0
        u.pin("CLOCK").drive<Level>(Level::High); // beginCycle of next cycle propagates Q pin
        CHECK(u.inspect("Q")->value == 0u);
        CHECK(u.pin("Q").read<Level>() == Level::Low);
    }

    TEST_CASE("SAV: T->M(R(X)), nMWR pulses")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x78); // fetch SAV
        clockN(u, 8);
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
        u.pin("CLOCK").drive<Level>(Level::High);
        CHECK(sawNMWRLow);
        CHECK(u.pin("nMWR").read<Level>() == Level::High);
    }

    TEST_CASE("MARK: (X,P)->T, P->X, R(2)--, nMWR pulses")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // After reset: X=0, P=0, R(2)=0x0000. Fetch MARK: R(0)=0x0001.
        // MARK: T=(0<<4)|0=0x00, write 0x00->M(R(2)=0x0000), X=P=0, R(2)--=0xFFFF
        driveBus(u, 0x79); // fetch MARK
        clockN(u, 8);
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
        u.pin("CLOCK").drive<Level>(Level::High);

        CHECK(u.inspect("T")->value == 0x00u);
        CHECK(u.inspect("X")->value == 0u);
        CHECK(u.inspect("P")->value == 0u);
        CHECK(u.inspect("R2")->value == 0xFFFFu);
        CHECK(sawNMWRLow);
        CHECK(u.pin("nMWR").read<Level>() == Level::High);
    }

    TEST_CASE("RET: M(R(X))->(X,P), R(X)++, IE=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // X=0, R(X=0)=0x0001 after fetch, RET reads bus value 0x23 -> X=2, P=3, R(0)++
        driveBus(u, 0x70); // fetch RET
        clockN(u, 8);
        driveBus(u, 0x23); // M(R(X)) = 0x23 -> X=2, P=3
        clockN(u, 8);
        CHECK(u.inspect("X")->value == 2u);
        CHECK(u.inspect("P")->value == 3u);
        CHECK(u.inspect("IE")->value == 1u);
    }

    TEST_CASE("DIS: M(R(X))->(X,P), R(X)++, IE=0")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x71); // fetch DIS
        clockN(u, 8);
        driveBus(u, 0x23); // M(R(X)) = 0x23 -> X=2, P=3
        clockN(u, 8);
        CHECK(u.inspect("X")->value == 2u);
        CHECK(u.inspect("P")->value == 3u);
        CHECK(u.inspect("IE")->value == 0u);
    }
}

TEST_SUITE("chip::cdp1xxx::CDP1802 IOByteTransfer")
{
    TEST_CASE("OUT1: M(R(X))->BUS, R(X)++, N=1")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        // X=0, R(X=0)=0x0001 after fetch, OUT1 reads bus then R(X)++ -> 0x0002
        driveBus(u, 0x61); // fetch OUT1
        clockN(u, 8);
        driveBus(u, 0xAB); // M(R(X)) on bus during execute
        clockN(u, 8);
        CHECK(u.inspect("R0")->value == 0x0002u);
    }

    TEST_CASE("OUT1: N lines = 1 during execute cycle")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x61); // fetch OUT1
        clockN(u, 8);
        driveBus(u, 0x00);

        bool sawN1 = false;
        for (int i = 0; i < 8; ++i)
        {
            u.pin("CLOCK").drive<Level>(Level::High);
            if (u.pin("N0").read<Level>() == Level::High && u.pin("N1").read<Level>() == Level::Low &&
                u.pin("N2").read<Level>() == Level::Low)
            {
                sawN1 = true;
            }
            u.pin("CLOCK").drive<Level>(Level::Low);
        }
        CHECK(sawN1);
    }

    TEST_CASE("INP1: BUS->D, nMWR pulses")
    {
        CDP1802 u{ CDP1802::Descriptor{ .name = "U1" } };
        power(u);
        u.pin("nCLEAR").drive<Level>(Level::High);

        driveBus(u, 0x69); // fetch INP1
        clockN(u, 8);
        driveBus(u, 0x5A); // data on bus during execute -> D=0x5A

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
        u.pin("CLOCK").drive<Level>(Level::High);
        CHECK(u.inspect("D")->value == 0x5Au);
        CHECK(sawNMWRLow);
        CHECK(u.pin("nMWR").read<Level>() == Level::High);
    }
}
