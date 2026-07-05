#include <doctest/doctest.h>

#include <nfx/Silicon.h>

using namespace nfx::silicon;
using namespace nfx::silicon::discrete;
using namespace nfx::silicon::signal;

TEST_SUITE("discrete::PowerSupply")
{
    TEST_CASE("Construction creates one pin per rail")
    {
        PowerRail rails[] = { VDD, VSS };
        PowerSupply psu{ PowerSupply::Descriptor{ .name = "PSU", .rails = rails } };

        CHECK(psu.pins().size() == 2);
    }

    TEST_CASE("Pins are named after their rails")
    {
        PowerRail rails[] = { VDD, VSS };
        PowerSupply psu{ PowerSupply::Descriptor{ .name = "PSU", .rails = rails } };

        CHECK(psu.pin("VDD").descriptor().name == std::string_view("VDD"));
        CHECK(psu.pin("VSS").descriptor().name == std::string_view("VSS"));
    }

    TEST_CASE("Pins are Analog Output")
    {
        PowerRail rails[] = { VDD };
        PowerSupply psu{ PowerSupply::Descriptor{ .name = "PSU", .rails = rails } };

        CHECK(psu.pin("VDD").descriptor().kind == Pin::Kind::Analog);
        CHECK(psu.pin("VDD").descriptor().direction == Pin::Direction::Output);
    }

    TEST_CASE("Pins drive the correct voltage")
    {
        PowerRail rails[] = { VDD, VSS, VBB };
        PowerSupply psu{ PowerSupply::Descriptor{ .name = "PSU", .rails = rails } };

        REQUIRE(psu.pin("VDD").read<float>().has_value());
        CHECK(psu.pin("VDD").read<float>().value() == 5.0f);

        REQUIRE(psu.pin("VSS").read<float>().has_value());
        CHECK(psu.pin("VSS").read<float>().value() == 0.0f);

        REQUIRE(psu.pin("VBB").read<float>().has_value());
        CHECK(psu.pin("VBB").read<float>().value() == 12.0f);
    }

    TEST_CASE("Negative rail voltage")
    {
        PowerRail rails[] = { VEE };
        PowerSupply psu{ PowerSupply::Descriptor{ .name = "PSU", .rails = rails } };

        REQUIRE(psu.pin("VEE").read<float>().has_value());
        CHECK(psu.pin("VEE").read<float>().value() == -12.0f);
    }

    TEST_CASE("Custom rail")
    {
        constexpr PowerRail V3_3{ "V3.3", 3.3f };
        PowerRail rails[] = { V3_3 };
        PowerSupply psu{ PowerSupply::Descriptor{ .name = "PSU", .rails = rails } };

        REQUIRE(psu.pin("V3.3").read<float>().has_value());
        CHECK(psu.pin("V3.3").read<float>().value() == 3.3f);
    }

    TEST_CASE("Component name is stored correctly")
    {
        PowerRail rails[] = { VDD };
        PowerSupply psu(PowerSupply::Descriptor{ .name = "MainPSU", .rails = rails });

        CHECK(psu.name() == std::string_view("MainPSU"));
    }
}
