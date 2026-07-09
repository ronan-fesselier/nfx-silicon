#pragma once

/**
 * \file Types.h
 * \brief Declares core scalar types shared across nfx::silicon contracts.
 */

#include <compare>
#include <cstdint>
#include <functional>
#include <optional>

namespace nfx::silicon
{
    /**
     * \brief Logical levels for hardware pins and signals.
     */
    enum class Level : std::uint8_t
    {
        Low = 0, // VSS (ground, 0)
        High,    // VDD/VCC (supply, 1)
        HighZ    // High-impedance (tristate, not driven)
    };

    using Cycles = std::uint32_t;         ///< Cycle count consumed by one instruction
    using Tick = std::uint64_t;           ///< Cumulative simulation clock counter
    using Voltage = std::optional<float>; ///< Analog voltage, nullopt = floating
    using AddressOffset = std::uint64_t;  ///< Offset type for address arithmetic

    /**
     * \struct Address
     * \brief Strong 64-bit address type used across all CPU and memory contracts.
     */
    struct Address final
    {
        std::uint64_t value{ 0 }; ///< raw address value

        Address() = default;

        /**
         * \brief Constructs an address from a raw value.
         * \param raw Raw address value.
         */
        constexpr explicit Address(const std::uint64_t raw)
            : value{ raw }
        {}

        /**
         * \brief Returns the raw address value.
         */
        [[nodiscard]] constexpr std::uint64_t raw() const { return value; }

        constexpr bool operator==(const Address&) const = default;
        constexpr auto operator<=>(const Address&) const = default;
    };

    [[nodiscard]] constexpr Address operator+(const Address address, const AddressOffset offset)
    {
        return Address{ address.value + offset };
    }

    [[nodiscard]] constexpr Address operator-(const Address address, const AddressOffset offset)
    {
        return Address{ address.value - offset };
    }

    [[nodiscard]] constexpr Address operator&(const Address address, const std::uint64_t mask)
    {
        return Address{ address.value & mask };
    }

} // namespace nfx::silicon

template <>
struct std::hash<nfx::silicon::Address>
{
    std::size_t operator()(const nfx::silicon::Address& address) const noexcept
    {
        return std::hash<std::uint64_t>{}(address.raw());
    }
};
