#pragma once

/**
 * \file CD4515B.h
 * \brief Declares the CD4515B 4-bit latch / 1-of-16 decoder package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::decoder::cd4xxx
{
    /**
     * \class CD4515B
     * \brief CD4515B 4-bit latch / 1-of-16 decoder in a DIP-24 package.
     *
     * Decodes a 4-bit address (DATA1..DATA4 = A..D) to one of sixteen
     * active-Low outputs (S0..S15). When STROBE is High the decoder is
     * transparent; when STROBE goes Low the address is latched and outputs
     * hold their state. INHIBIT High forces all outputs High regardless of
     * the address.
     *
     * CMOS logic family (CD4000B series). Operates from 3V to 18V VDD.
     * Outputs are gated by VDD: if VDD is not within [3.0V, 18.0V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pin names follow the RCA CD4515B datasheet (92CS-24554).
     *
     * Pinout (DIP-24):
     *   1  = STROBE    24 = VDD
     *   2  = DATA1     23 = INHIBIT
     *   3  = DATA2     22 = DATA4
     *   4  = S7        21 = DATA3
     *   5  = S6        20 = S10
     *   6  = S5        19 = S11
     *   7  = S4        18 = S8
     *   8  = S3        17 = S9
     *   9  = S1        16 = S14
     *  10  = S2        15 = S15
     *  11  = S0        14 = S12
     *  12  = VSS       13 = S13
     *
     * Truth table (STROBE = High):
     *   INHIBIT | D | C | B | A | Active output (Low)
     *   --------|---|---|---|---|--------------------
     *   Low     | 0 | 0 | 0 | 0 | S0
     *   Low     | 0 | 0 | 0 | 1 | S1
     *   ...
     *   Low     | 1 | 1 | 1 | 1 | S15
     *   High    | X | X | X | X | none (all High)
     */
    class CD4515B final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-24 terminals of the CD4515B.
         */
        enum class Terminal : std::uint8_t
        {
            STROBE, ///< pin 1  Latch strobe (High = transparent, Low = latched)
            DATA1,  ///< pin 2  Address input A (LSB)
            DATA2,  ///< pin 3  Address input B
            S7,     ///< pin 4  Decoder output 7 (active-Low)
            S6,     ///< pin 5  Decoder output 6 (active-Low)
            S5,     ///< pin 6  Decoder output 5 (active-Low)
            S4,     ///< pin 7  Decoder output 4 (active-Low)
            S3,     ///< pin 8  Decoder output 3 (active-Low)
            S1,     ///< pin 9  Decoder output 1 (active-Low)
            S2,     ///< pin 10 Decoder output 2 (active-Low)
            S0,     ///< pin 11 Decoder output 0 (active-Low)
            VSS,    ///< pin 12 Ground

            S13,     ///< pin 13 Decoder output 13 (active-Low)
            S12,     ///< pin 14 Decoder output 12 (active-Low)
            S15,     ///< pin 15 Decoder output 15 (active-Low)
            S14,     ///< pin 16 Decoder output 14 (active-Low)
            S9,      ///< pin 17 Decoder output 9 (active-Low)
            S8,      ///< pin 18 Decoder output 8 (active-Low)
            S11,     ///< pin 19 Decoder output 11 (active-Low)
            S10,     ///< pin 20 Decoder output 10 (active-Low)
            DATA3,   ///< pin 21 Address input C
            DATA4,   ///< pin 22 Address input D (MSB)
            INHIBIT, ///< pin 23 Inhibit (active-High, forces all outputs High)
            VDD      ///< pin 24 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CD4515B construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U12")
        };

        /**
         * \brief Constructs a CD4515B from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CD4515B(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("STROBE", "INHIBIT", "DATA1".."DATA4",
         *             "S0".."S15", "VDD", "VSS").
         */
        signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 24 pins in DIP-24 order.
         */
        [[nodiscard]] std::span<signal::Pin* const> pins() const override;

    private:
        void onVDD(Voltage v);
        void compute();

        static constexpr float k_vddMin = 3.0f;  ///< Minimum valid VDD (V)
        static constexpr float k_vddMax = 18.0f; ///< Maximum valid VDD (V)

        Descriptor m_descriptor;
        bool m_powered{ false };
        std::uint8_t m_latch{ 0 };

        signal::Pin m_pinSTROBE;
        signal::Pin m_pinDATA1;
        signal::Pin m_pinDATA2;
        signal::Pin m_pinS7;
        signal::Pin m_pinS6;
        signal::Pin m_pinS5;
        signal::Pin m_pinS4;
        signal::Pin m_pinS3;
        signal::Pin m_pinS1;
        signal::Pin m_pinS2;
        signal::Pin m_pinS0;
        signal::Pin m_pinVSS;

        signal::Pin m_pinS13;
        signal::Pin m_pinS12;
        signal::Pin m_pinS15;
        signal::Pin m_pinS14;
        signal::Pin m_pinS9;
        signal::Pin m_pinS8;
        signal::Pin m_pinS11;
        signal::Pin m_pinS10;
        signal::Pin m_pinDATA3;
        signal::Pin m_pinDATA4;
        signal::Pin m_pinINHIBIT;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 24> m_pinPtrs;
        std::array<signal::Pin*, 16> m_sPtrs; ///< S0..S15 in order
    };
} // namespace nfx::silicon::decoder::cd4xxx
