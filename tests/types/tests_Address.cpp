#include <doctest/doctest.h>

#include <nfx/Silicon.h>

#include <functional>

using namespace nfx::silicon;

TEST_SUITE("Address")
{
    TEST_CASE("Default construction yields zero")
    {
        const Address a;
        CHECK(a.raw() == 0u);
    }

    TEST_CASE("Construction from raw value")
    {
        const Address a{ 0x1234u };
        CHECK(a.raw() == 0x1234u);
    }

    TEST_CASE("Equality")
    {
        const Address a{ 0xABCDu };
        const Address b{ 0xABCDu };
        const Address c{ 0x0001u };

        CHECK(a == b);
        CHECK(a != c);
    }

    TEST_CASE("Ordering")
    {
        const Address lo{ 0x0000u };
        const Address hi{ 0xFFFFu };

        CHECK(lo < hi);
        CHECK(hi > lo);
        CHECK(lo <= lo);
        CHECK(hi >= hi);
    }

    TEST_CASE("Addition with offset")
    {
        const Address a{ 0x1000u };
        const Address b = a + 0x100u;

        CHECK(b.raw() == 0x1100u);
    }

    TEST_CASE("Subtraction with offset")
    {
        const Address a{ 0x1100u };
        const Address b = a - 0x100u;

        CHECK(b.raw() == 0x1000u);
    }

    TEST_CASE("Bitwise AND with mask")
    {
        const Address a{ 0xFFFFu };
        const Address b = a & 0x0F0Fu;

        CHECK(b.raw() == 0x0F0Fu);
    }

    TEST_CASE("std::hash is consistent for equal addresses")
    {
        const Address a{ 0xDEADBEEFu };
        const Address b{ 0xDEADBEEFu };

        CHECK(std::hash<Address>{}(a) == std::hash<Address>{}(b));
    }

    TEST_CASE("std::hash differs for distinct addresses")
    {
        const Address a{ 0x0000u };
        const Address b{ 0xFFFFu };

        CHECK(std::hash<Address>{}(a) != std::hash<Address>{}(b));
    }
}
