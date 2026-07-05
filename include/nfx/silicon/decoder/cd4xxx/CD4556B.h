#pragma once

/**
 * \file CD4556B.h
 * \brief Declares the CD4556B dual binary 1-of-4 decoder/demultiplexer package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::decoder::cd4xxx
{
    /**
     * \class CD4556B
     * \brief CD4556B dual binary 1-of-4 decoder/demultiplexer in a DIP-16 package.
     *
     * Two independent 1-of-4 decoders. Each decoder has two select inputs
     * (A, B), an active-Low enable input (/E), and four mutually exclusive
     * active-Low outputs (/Q0../Q3). When /E is High, all outputs remain High
     * regardless of A/B.
     *
     * CMOS logic family (CD4000B series). Operates from 3V to 18V VDD.
     * Outputs are gated by VDD: if VDD is not within [3.0V, 18.0V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pin names follow the RCA CD4556B datasheet (92CS-24943RI).
     *
     * Pinout (DIP-16):
     *   1  = /E1    16 = VDD
     *   2  = A1     15 = /E2
     *   3  = B1     14 = A2
     *   4  = /1Q0   13 = B2
     *   5  = /1Q1   12 = /2Q0
     *   6  = /1Q2   11 = /2Q1
     *   7  = /1Q3   10 = /2Q2
     *   8  = VSS     9 = /2Q3
     *
     * Truth table (each decoder, CD4556B):
     *   /E | B | A | /Q0  | /Q1  | /Q2  | /Q3
     *   ---|---|---|------|------|------|------
     *   0  | 0 | 0 | Low  | High | High | High
     *   0  | 0 | 1 | High | Low  | High | High
     *   0  | 1 | 0 | High | High | Low  | High
     *   0  | 1 | 1 | High | High | High | Low
     *   1  | X | X | High | High | High | High
     */
    class CD4556B final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-16 terminals of the CD4556B.
         */
        enum class Terminal : std::uint8_t
        {
            nE1,   ///< pin 1  Decoder 1 active-Low enable
            A1,    ///< pin 2  Decoder 1 select input A
            B1,    ///< pin 3  Decoder 1 select input B
            nQ1_0, ///< pin 4  Decoder 1 output 0 (active-Low)
            nQ1_1, ///< pin 5  Decoder 1 output 1 (active-Low)
            nQ1_2, ///< pin 6  Decoder 1 output 2 (active-Low)
            nQ1_3, ///< pin 7  Decoder 1 output 3 (active-Low)
            VSS,   ///< pin 8  Ground

            nQ2_3, ///< pin 9  Decoder 2 output 3 (active-Low)
            nQ2_2, ///< pin 10 Decoder 2 output 2 (active-Low)
            nQ2_1, ///< pin 11 Decoder 2 output 1 (active-Low)
            nQ2_0, ///< pin 12 Decoder 2 output 0 (active-Low)
            B2,    ///< pin 13 Decoder 2 select input B
            A2,    ///< pin 14 Decoder 2 select input A
            nE2,   ///< pin 15 Decoder 2 active-Low enable
            VDD    ///< pin 16 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CD4556B construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U9")
        };

        /**
         * \brief Constructs a CD4556B from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CD4556B(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("/E1","A1","B1","/1Q0".."/1Q3",
         *             "/E2","A2","B2","/2Q0".."/2Q3","VDD","VSS").
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
        void compute(std::uint8_t decoder);

        static constexpr float k_vddMin = 3.0f;  ///< Minimum valid VDD (V)
        static constexpr float k_vddMax = 18.0f; ///< Maximum valid VDD (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        signal::Pin m_pinNE1;
        signal::Pin m_pinA1;
        signal::Pin m_pinB1;
        signal::Pin m_pinNQ1_0;
        signal::Pin m_pinNQ1_1;
        signal::Pin m_pinNQ1_2;
        signal::Pin m_pinNQ1_3;
        signal::Pin m_pinVSS;

        signal::Pin m_pinNQ2_0;
        signal::Pin m_pinNQ2_1;
        signal::Pin m_pinNQ2_2;
        signal::Pin m_pinNQ2_3;
        signal::Pin m_pinB2;
        signal::Pin m_pinA2;
        signal::Pin m_pinNE2;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 16> m_pinPtrs;
    };
} // namespace nfx::silicon::decoder::cd4xxx
