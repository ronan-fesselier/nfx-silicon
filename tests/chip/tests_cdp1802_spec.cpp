#include <doctest/doctest.h>

#include <nfx/Silicon.h>

#include "chip/cdp1xxx/internal/CDP1802/Spec.h"

using namespace nfx::silicon;
using namespace nfx::silicon::chip::cdp1xxx;
using namespace nfx::silicon::chip::cdp1xxx::cdp1802internal;

TEST_CASE("cdp1802Spec: opcode 0x00 returns IDL not LDN")
{
    const auto& spec = cdp1802Spec(0x00);
    CHECK(std::string_view(spec.mnemonic) == "IDL");
}

TEST_CASE("cdp1802Spec: opcode 0x01 returns LDN")
{
    const auto& spec = cdp1802Spec(0x01);
    CHECK(std::string_view(spec.mnemonic) == "LDN");
}

TEST_CASE("cdp1802Spec: spot-check a full-match opcode (LDI 0xF8)")
{
    const auto& spec = cdp1802Spec(0xF8);
    CHECK(std::string_view(spec.mnemonic) == "LDI");
}

TEST_CASE("cdp1802Spec: spot-check a nibble-high opcode (SEP 0xD3)")
{
    const auto& spec = cdp1802Spec(0xD3);
    CHECK(std::string_view(spec.mnemonic) == "SEP");
}

TEST_CASE("cdp1802Spec: unknown opcode returns UNK fallback")
{
    const auto& spec = cdp1802Spec(0x68);
    CHECK(std::string_view(spec.mnemonic) == "UNK");
}
