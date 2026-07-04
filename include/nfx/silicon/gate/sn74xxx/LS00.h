#pragma once

/**
 * \file LS00.h
 * \brief Declares the SN74LS00 quad 2-input NAND gate package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/gate/Nand.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::gate::sn74
{
    /**
     * \class LS00
     * \brief SN74LS00 quad 2-input NAND gate: four independent Nand gates in a DIP-14 package.
     *
     * Encapsulates four Nand gate primitives behind a DIP-14 pin interface.
     * Outputs are gated by VCC: if VCC is not within [4.75V, 5.25V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pinout (DIP-14):
     *   1  = 1A    14 = VCC
     *   2  = 1B    13 = 4A
     *   3  = 1Y    12 = 4B
     *   4  = 2A    11 = 4Y
     *   5  = 2B    10 = 3A
     *   6  = 2Y     9 = 3B
     *   7  = GND    8 = 3Y
     *
     * Truth table (each gate):
     *   A     | B     | Y
     *   ------|-------|------
     *   Low   | Low   | High
     *   Low   | High  | High
     *   High  | Low   | High
     *   High  | High  | Low
     *   HighZ | any   | HighZ
     *   any   | HighZ | HighZ
     */
    class LS00 final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-14 terminals of the LS00.
         */
        enum class Terminal : std::uint8_t
        {
            A1,  ///< pin 1  Gate 1 input A
            B1,  ///< pin 2  Gate 1 input B
            Y1,  ///< pin 3  Gate 1 output
            A2,  ///< pin 4  Gate 2 input A
            B2,  ///< pin 5  Gate 2 input B
            Y2,  ///< pin 6  Gate 2 output
            GND, ///< pin 7  Ground

            Y3, ///< pin 8  Gate 3 output
            B3, ///< pin 10 Gate 3 input B
            A3, ///< pin 9  Gate 3 input A
            Y4, ///< pin 11 Gate 4 output
            B4, ///< pin 13 Gate 4 input B
            A4, ///< pin 12 Gate 4 input A
            VCC ///< pin 14 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LS00 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an LS00 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit LS00(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("1A".."4A", "1B".."4B", "1Y".."4Y", "VCC", "GND").
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

        std::array<Nand, 4> m_gates;

        signal::Pin m_pinA1;
        signal::Pin m_pinB1;
        signal::Pin m_pinY1;
        signal::Pin m_pinA2;
        signal::Pin m_pinB2;
        signal::Pin m_pinY2;
        signal::Pin m_pinGND;

        signal::Pin m_pinY3;
        signal::Pin m_pinB3;
        signal::Pin m_pinA3;
        signal::Pin m_pinY4;
        signal::Pin m_pinB4;
        signal::Pin m_pinA4;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 14> m_pinPtrs;
    };
} // namespace nfx::silicon::gate::sn74
