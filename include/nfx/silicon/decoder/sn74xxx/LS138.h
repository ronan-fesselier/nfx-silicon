#pragma once

/**
 * \file LS138.h
 * \brief Declares the SN74LS138 3-to-8 line decoder/demultiplexer package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::decoder::sn74xxx
{
    /**
     * \class LS138
     * \brief SN74LS138 3-to-8 line decoder/demultiplexer in a DIP-16 package.
     *
     * Decodes a 3-bit binary address (A, B, C) to one of eight active-Low outputs
     * (Y0..Y7). All outputs are High when disabled. Enable logic: G1=High,
     * /G2A=Low, /G2B=Low. Any other combination disables all outputs (all High).
     *
     * Outputs are gated by VCC: if VCC is not within [4.75V, 5.25V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pinout (DIP-16):
     *   1  = A      16 = VCC
     *   2  = B      15 = Y0
     *   3  = C      14 = Y1
     *   4  = /G2A   13 = Y2
     *   5  = /G2B   12 = Y3
     *   6  = G1     11 = Y4
     *   7  = Y7     10 = Y5
     *   8  = GND     9 = Y6
     *
     * Truth table (Y0..Y7 active-Low, X = don't care):
     *   G1  | /G2A | /G2B | C | B | A | active output
     *   ----|------|------|---|---|---|---------------
     *   Low |  X   |  X   | X | X | X | none (all High)
     *   X   | High |  X   | X | X | X | none (all High)
     *   X   |  X   | High | X | X | X | none (all High)
     *   High| Low  | Low  | 0 | 0 | 0 | Y0 Low, others High
     *   High| Low  | Low  | 0 | 0 | 1 | Y1 Low, others High
     *   High| Low  | Low  | 0 | 1 | 0 | Y2 Low, others High
     *   High| Low  | Low  | 0 | 1 | 1 | Y3 Low, others High
     *   High| Low  | Low  | 1 | 0 | 0 | Y4 Low, others High
     *   High| Low  | Low  | 1 | 0 | 1 | Y5 Low, others High
     *   High| Low  | Low  | 1 | 1 | 0 | Y6 Low, others High
     *   High| Low  | Low  | 1 | 1 | 1 | Y7 Low, others High
     */
    class LS138 final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-16 terminals of the LS138.
         */
        enum class Terminal : std::uint8_t
        {
            A,    ///< pin 1  Address input A (LSB)
            B,    ///< pin 2  Address input B
            C,    ///< pin 3  Address input C (MSB)
            nG2A, ///< pin 4  Active-Low enable input /G2A
            nG2B, ///< pin 5  Active-Low enable input /G2B
            G1,   ///< pin 6  Active-High enable input G1
            Y7,   ///< pin 7  Output 7 (active-Low)
            GND,  ///< pin 8  Ground

            Y6, ///< pin 9  Output 6 (active-Low)
            Y5, ///< pin 10 Output 5 (active-Low)
            Y4, ///< pin 11 Output 4 (active-Low)
            Y3, ///< pin 12 Output 3 (active-Low)
            Y2, ///< pin 13 Output 2 (active-Low)
            Y1, ///< pin 14 Output 1 (active-Low)
            Y0, ///< pin 15 Output 0 (active-Low)
            VCC ///< pin 16 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LS138 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an LS138 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit LS138(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("A", "B", "C", "/G2A", "/G2B", "G1", "Y0".."Y7", "VCC", "GND").
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
        void compute();

        static constexpr float k_vccMin = 4.75f; ///< Minimum valid VCC (V)
        static constexpr float k_vccMax = 5.25f; ///< Maximum valid VCC (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        signal::Pin m_pinA;
        signal::Pin m_pinB;
        signal::Pin m_pinC;
        signal::Pin m_pinNG2A;
        signal::Pin m_pinNG2B;
        signal::Pin m_pinG1;
        signal::Pin m_pinY7;
        signal::Pin m_pinGND;

        signal::Pin m_pinY6;
        signal::Pin m_pinY5;
        signal::Pin m_pinY4;
        signal::Pin m_pinY3;
        signal::Pin m_pinY2;
        signal::Pin m_pinY1;
        signal::Pin m_pinY0;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 16> m_pinPtrs;
    };
} // namespace nfx::silicon::decoder::sn74xxx
