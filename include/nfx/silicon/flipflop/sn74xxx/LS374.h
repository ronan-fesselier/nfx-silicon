#pragma once

/**
 * \file LS374.h
 * \brief Declares the SN74LS374 octal D-type flip-flop package.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/primitive/DFlipFlop.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::flipflop::sn74xxx
{
    /**
     * \class LS374
     * \brief SN74LS374 octal D-type edge-triggered flip-flop with 3-STATE outputs.
     *
     * Encapsulates eight DFlipFlop primitives behind a DIP-20 pin interface.
     * Outputs are gated by VCC: if VCC is not within [4.75V, 5.25V],
     * all outputs are held at HighZ and inputs are ignored.
     * OC is active Low. When OC is High, all Q outputs go to HighZ regardless
     * of CLK and D inputs.
     *
     * Pinout (DIP-20):
     *   1  = OC    20 = VCC
     *   2  = 1Q    19 = 8Q
     *   3  = 1D    18 = 8D
     *   4  = 2D    17 = 7D
     *   5  = 2Q    16 = 7Q
     *   6  = 3Q    15 = 6Q
     *   7  = 3D    14 = 6D
     *   8  = 4D    13 = 5D
     *   9  = 4Q    12 = 5Q
     *  10  = GND   11 = CLK
     *
     * Truth table (per flip-flop):
     *   OC   | CLK         | D    | Q
     *   -----|-------------|------|-----
     *   Low  | rising edge | Low  | Low   (sampled on rising edge)
     *   Low  | rising edge | High | High
     *   Low  | no edge     | any  | hold
     *   High | any         | any  | Hi-Z  (output disable)
     */
    class LS374 final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-20 terminals of the LS374 (physical pin order).
         */
        enum class Terminal : std::uint8_t
        {
            OC,  ///< pin 1  Output Control (active Low)
            Q1,  ///< pin 2  FF1 non-inverting output
            D1,  ///< pin 3  FF1 data input
            D2,  ///< pin 4  FF2 data input
            Q2,  ///< pin 5  FF2 output
            Q3,  ///< pin 6  FF3 output
            D3,  ///< pin 7  FF3 data input
            D4,  ///< pin 8  FF4 data input
            Q4,  ///< pin 9  FF4 output
            GND, ///< pin 10 Ground

            CLK, ///< pin 11 Clock input (rising edge triggered)
            Q5,  ///< pin 12 FF5 output
            D5,  ///< pin 13 FF5 data input
            D6,  ///< pin 14 FF6 data input
            Q6,  ///< pin 15 FF6 output
            Q7,  ///< pin 16 FF7 output
            D7,  ///< pin 17 FF7 data input
            D8,  ///< pin 18 FF8 data input
            Q8,  ///< pin 19 FF8 output
            VCC  ///< pin 20 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LS374 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an LS374 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit LS374(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("OC", "CLK", "1Q".."8Q", "1D".."8D", "VCC", "GND").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 20 pins in DIP-20 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets all eight flip-flops: drives Q=Low on each.
         */
        virtual void reset() override;

    private:
        void onVCC(Voltage v);
        void onOC(Level level);

        static constexpr float k_vccMin = 4.75f; ///< Minimum valid VCC (V)
        static constexpr float k_vccMax = 5.25f; ///< Maximum valid VCC (V)

        Descriptor m_descriptor;
        bool m_powered{ false };
        bool m_outputsEnabled{ false };

        std::array<primitive::DFlipFlop, 8> m_flipflops;

        signal::Pin m_pinOC;
        signal::Pin m_pinQ1;
        signal::Pin m_pinD1;
        signal::Pin m_pinD2;
        signal::Pin m_pinQ2;
        signal::Pin m_pinQ3;
        signal::Pin m_pinD3;
        signal::Pin m_pinD4;
        signal::Pin m_pinQ4;
        signal::Pin m_pinGND;

        signal::Pin m_pinCLK;
        signal::Pin m_pinQ5;
        signal::Pin m_pinD5;
        signal::Pin m_pinD6;
        signal::Pin m_pinQ6;
        signal::Pin m_pinQ7;
        signal::Pin m_pinD7;
        signal::Pin m_pinD8;
        signal::Pin m_pinQ8;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 20> m_pinPtrs;
    };
} // namespace nfx::silicon::flipflop::sn74xxx
