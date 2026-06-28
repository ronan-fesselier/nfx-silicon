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

    using Voltage = std::optional<float>; ///< Analog voltage, nullopt = floating
} // namespace nfx::silicon
