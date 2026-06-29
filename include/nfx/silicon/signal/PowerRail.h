#pragma once

/**
 * \file PowerRail.h
 * \brief Declares the PowerRail struct and predefined voltage constants.
 */

namespace nfx::silicon::signal
{
    /**
     * \struct PowerRail
     * \brief Named voltage reference constant.
     */
    struct PowerRail
    {
        const char* name; ///< Identifier (e.g. "VDD", "VSS")
        float voltage;    ///< Voltage in volts (can be negative)
    };

    inline constexpr PowerRail VSS{ "VSS", 0.0f };
    inline constexpr PowerRail GND{ "GND", 0.0f };
    inline constexpr PowerRail AGND{ "AGND", 0.0f };
    inline constexpr PowerRail VDD{ "VDD", 5.0f };
    inline constexpr PowerRail VCC{ "VCC", 5.0f };
    inline constexpr PowerRail AVCC{ "AVCC", 5.0f };
    inline constexpr PowerRail VBB{ "VBB", 12.0f };
    inline constexpr PowerRail VEE{ "VEE", -12.0f };
} // namespace nfx::silicon::signal
