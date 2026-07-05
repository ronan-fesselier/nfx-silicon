#pragma once

/**
 * \file LS139.h
 * \brief Declares the SN74LS139 dual 2-to-4 line decoder/demultiplexer package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::decoder::sn74xxx
{
    /**
     * \class LS139
     * \brief SN74LS139 dual 2-to-4 line decoder/demultiplexer in a DIP-16 package.
     *
     * Contains two independent 2-to-4 decoders. Each decoder takes a 2-bit
     * address (A, B) and an active-Low enable (/G). When enabled, exactly one
     * of the four active-Low outputs (Y0..Y3) is driven Low. When disabled
     * (/G=High), all outputs are High.
     *
     * Outputs are gated by VCC: if VCC is not within [4.5V, 5.5V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pinout (DIP-16):
     *   1  = /1G    16 = VCC
     *   2  = 1A     15 = /2G
     *   3  = 1B     14 = 2A
     *   4  = 1Y0    13 = 2B
     *   5  = 1Y1    12 = 2Y0
     *   6  = 1Y2    11 = 2Y1
     *   7  = 1Y3    10 = 2Y2
     *   8  = GND     9 = 2Y3
     *
     * Truth table (each decoder, active-Low outputs):
     *   /G  | B | A | Y0 | Y1 | Y2 | Y3
     *   ----|---|---|----|----|----|----
     *   High| X | X |  H |  H |  H |  H
     *   Low | L | L |  L |  H |  H |  H
     *   Low | L | H |  H |  L |  H |  H
     *   Low | H | L |  H |  H |  L |  H
     *   Low | H | H |  H |  H |  H |  L
     */
    class LS139 final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-16 terminals of the LS139.
         */
        enum class Terminal : std::uint8_t
        {
            nG1,  ///< pin 1  Decoder 1 active-Low enable
            A1,   ///< pin 2  Decoder 1 address input A
            B1,   ///< pin 3  Decoder 1 address input B
            Y1_0, ///< pin 4  Decoder 1 output 0 (active-Low)
            Y1_1, ///< pin 5  Decoder 1 output 1 (active-Low)
            Y1_2, ///< pin 6  Decoder 1 output 2 (active-Low)
            Y1_3, ///< pin 7  Decoder 1 output 3 (active-Low)
            GND,  ///< pin 8  Ground

            Y2_3, ///< pin 9  Decoder 2 output 3 (active-Low)
            Y2_2, ///< pin 10 Decoder 2 output 2 (active-Low)
            Y2_1, ///< pin 11 Decoder 2 output 1 (active-Low)
            Y2_0, ///< pin 12 Decoder 2 output 0 (active-Low)
            B2,   ///< pin 13 Decoder 2 address input B
            A2,   ///< pin 14 Decoder 2 address input A
            nG2,  ///< pin 15 Decoder 2 active-Low enable
            VCC   ///< pin 16 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LS139 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an LS139 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit LS139(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("/1G", "1A", "1B", "1Y0".."1Y3",
         *             "/2G", "2A", "2B", "2Y0".."2Y3", "VCC", "GND").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 16 pins in DIP-16 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

    private:
        void onVCC(Voltage v);
        void compute(std::uint8_t decoder);

        static constexpr float k_vccMin = 4.5f; ///< Minimum valid VCC (V)
        static constexpr float k_vccMax = 5.5f; ///< Maximum valid VCC (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        signal::Pin m_pinNG1;
        signal::Pin m_pinA1;
        signal::Pin m_pinB1;
        signal::Pin m_pinY1_0;
        signal::Pin m_pinY1_1;
        signal::Pin m_pinY1_2;
        signal::Pin m_pinY1_3;
        signal::Pin m_pinGND;

        signal::Pin m_pinY2_3;
        signal::Pin m_pinY2_2;
        signal::Pin m_pinY2_1;
        signal::Pin m_pinY2_0;
        signal::Pin m_pinB2;
        signal::Pin m_pinA2;
        signal::Pin m_pinNG2;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 16> m_pinPtrs;
    };
} // namespace nfx::silicon::decoder::sn74xxx
