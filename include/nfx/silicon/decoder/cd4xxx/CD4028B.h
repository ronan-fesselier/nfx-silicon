#pragma once

/**
 * \file CD4028B.h
 * \brief Declares the CD4028B BCD-to-decimal decoder/driver package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::decoder::cd4xxx
{
    /**
     * \class CD4028B
     * \brief CD4028B CMOS BCD-to-decimal decoder/driver in a DIP-16 package.
     *
     * Four binary inputs (A, B, C, D) drive ten mutually exclusive active-High
     * outputs (Y0..Y9). BCD codes 0-9 assert the corresponding output High;
     * all other outputs remain Low. Invalid codes 10-15 force all outputs Low.
     *
     * CMOS logic family (CD4000B series). Operates from 3V to 18V VDD.
     * Outputs are gated by VDD: if VDD is not within [3.0V, 18.0V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pin names follow the RCA CD4028B datasheet.
     *
     * Pinout (DIP-16):
     *   1  = Y4    16 = VDD
     *   2  = Y2    15 = Y3
     *   3  = Y0    14 = Y1
     *   4  = Y7    13 = B
     *   5  = Y9    12 = C
     *   6  = Y5    11 = D
     *   7  = Y6    10 = A
     *   8  = VSS    9 = Y8
     *
     * Truth table (CD4028B):
     *   D | C | B | A | Active output
     *   --|---|---|---|---------------
     *   0 | 0 | 0 | 0 | Y0
     *   0 | 0 | 0 | 1 | Y1
     *   0 | 0 | 1 | 0 | Y2
     *   0 | 0 | 1 | 1 | Y3
     *   0 | 1 | 0 | 0 | Y4
     *   0 | 1 | 0 | 1 | Y5
     *   0 | 1 | 1 | 0 | Y6
     *   0 | 1 | 1 | 1 | Y7
     *   1 | 0 | 0 | 0 | Y8
     *   1 | 0 | 0 | 1 | Y9
     *   1 | 0 | 1 | 0 | (all Low)
     *   ...
     *   1 | 1 | 1 | 1 | (all Low)
     */
    class CD4028B final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-16 terminals of the CD4028B.
         */
        enum class Terminal : std::uint8_t
        {
            Y4,  ///< pin 1  Decimal output 4 (active-High)
            Y2,  ///< pin 2  Decimal output 2 (active-High)
            Y0,  ///< pin 3  Decimal output 0 (active-High)
            Y7,  ///< pin 4  Decimal output 7 (active-High)
            Y9,  ///< pin 5  Decimal output 9 (active-High)
            Y5,  ///< pin 6  Decimal output 5 (active-High)
            Y6,  ///< pin 7  Decimal output 6 (active-High)
            VSS, ///< pin 8  Ground

            Y8, ///< pin 9  Decimal output 8 (active-High)
            A,  ///< pin 10 BCD input bit 0 (LSB)
            D,  ///< pin 11 BCD input bit 3 (MSB)
            C,  ///< pin 12 BCD input bit 2
            B,  ///< pin 13 BCD input bit 1
            Y1, ///< pin 14 Decimal output 1 (active-High)
            Y3, ///< pin 15 Decimal output 3 (active-High)
            VDD ///< pin 16 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CD4028B construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U12")
        };

        /**
         * \brief Constructs a CD4028B from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CD4028B(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("A","B","C","D","Y0".."Y9","VDD","VSS").
         */
        signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 16 pins in DIP-16 order.
         */
        [[nodiscard]] std::span<signal::Pin* const> pins() const override;

    private:
        void onVDD(Voltage v);
        void compute();

        static constexpr float k_vddMin = 3.0f;  ///< Minimum valid VDD (V)
        static constexpr float k_vddMax = 18.0f; ///< Maximum valid VDD (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        signal::Pin m_pinY4;
        signal::Pin m_pinY2;
        signal::Pin m_pinY0;
        signal::Pin m_pinY7;
        signal::Pin m_pinY9;
        signal::Pin m_pinY5;
        signal::Pin m_pinY6;
        signal::Pin m_pinVSS;

        signal::Pin m_pinY8;
        signal::Pin m_pinA;
        signal::Pin m_pinD;
        signal::Pin m_pinC;
        signal::Pin m_pinB;
        signal::Pin m_pinY1;
        signal::Pin m_pinY3;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 16> m_pinPtrs;
        std::array<signal::Pin*, 10> m_yPtrs; ///< Y0..Y9 in order
    };
} // namespace nfx::silicon::decoder::cd4xxx
