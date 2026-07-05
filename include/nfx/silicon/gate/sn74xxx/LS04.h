#pragma once

/**
 * \file LS04.h
 * \brief Declares the SN74LS04 hex inverter package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/primitive/Not.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::gate::sn74xxx
{
    /**
     * \class LS04
     * \brief SN74LS04 hex inverter: six independent Not gates in a DIP-14 package.
     *
     * Encapsulates six Not gate primitives behind a DIP-14 pin interface.
     * Outputs are gated by VCC: if VCC is not within [4.75V, 5.25V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pinout (DIP-14):
     *   1  = 1A    14 = VCC
     *   2  = 1Y    13 = 6A
     *   3  = 2A    12 = 6Y
     *   4  = 2Y    11 = 5A
     *   5  = 3A    10 = 5Y
     *   6  = 3Y     9 = 4A
     *   7  = GND    8 = 4Y
     *
     * Truth table (each inverter):
     *   A     | Y
     *   ------|---------
     *   Low   | High
     *   High  | Low
     *   HighZ | HighZ
     */
    class LS04 final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-14 terminals of the LS04.
         */
        enum class Terminal : std::uint8_t
        {
            A1,  ///< pin 1  Inverter 1 input
            Y1,  ///< pin 2  Inverter 1 output
            A2,  ///< pin 3  Inverter 2 input
            Y2,  ///< pin 4  Inverter 2 output
            A3,  ///< pin 5  Inverter 3 input
            Y3,  ///< pin 6  Inverter 3 output
            GND, ///< pin 7  Ground

            Y4, ///< pin 8  Inverter 4 output
            A4, ///< pin 9  Inverter 4 input
            Y5, ///< pin 10 Inverter 5 output
            A5, ///< pin 11 Inverter 5 input
            Y6, ///< pin 12 Inverter 6 output
            A6, ///< pin 13 Inverter 6 input
            VCC ///< pin 14 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LS04 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an LS04 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit LS04(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("1A".."6A", "1Y".."6Y", "VCC", "GND").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 14 pins in DIP-14 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

    private:
        void onVCC(Voltage v);

        static constexpr float k_vccMin = 4.75f; ///< Minimum valid VCC (V)
        static constexpr float k_vccMax = 5.25f; ///< Maximum valid VCC (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        std::array<primitive::Not, 6> m_inverters;

        signal::Pin m_pinA1;
        signal::Pin m_pinY1;
        signal::Pin m_pinA2;
        signal::Pin m_pinY2;
        signal::Pin m_pinA3;
        signal::Pin m_pinY3;
        signal::Pin m_pinGND;

        signal::Pin m_pinY4;
        signal::Pin m_pinA4;
        signal::Pin m_pinY5;
        signal::Pin m_pinA5;
        signal::Pin m_pinY6;
        signal::Pin m_pinA6;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 14> m_pinPtrs;
    };
} // namespace nfx::silicon::gate::sn74xxx
