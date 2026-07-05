#pragma once

/**
 * \file LS74.h
 * \brief Declares the SN74LS74A dual D flip-flop package.
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
     * \class LS74
     * \brief SN74LS74A dual edge-triggered D flip-flop with asynchronous clear and preset.
     *
     * Encapsulates two DFlipFlop primitives behind a DIP-14 pin interface.
     * Outputs are gated by VCC: if VCC is not within the valid range [4.5V, 5.5V],
     * all outputs are held at HighZ and inputs are ignored.
     * CLR and PRE are active Low. Forbidden state (CLR=Low, PRE=Low) releases
     * Q and NQ to HighZ per datasheet "unstable" note.
     *
     * Pinout (DIP-14):
     *   1  = 1CLR    14 = VCC
     *   2  = 1D      13 = 2CLR
     *   3  = 1CLK    12 = 2D
     *   4  = 1PRE    11 = 2CLK
     *   5  = 1Q      10 = 2PRE
     *   6  = 1NQ      9  = 2Q
     *   7  = GND      8  = 2NQ
     *
     * Truth table (per flip-flop):
     *   CLR  | PRE  | CLK         | D    | Q     | NQ
     *   -----|------|-------------|------|-------|------
     *   Low  | High | any         | any  | Low   | High   (async clear)
     *   High | Low  | any         | any  | High  | Low    (async preset)
     *   Low  | Low  | any         | any  | HighZ | HighZ  (forbidden)
     *   High | High | rising edge | Low  | Low   | High
     *   High | High | rising edge | High | High  | Low
     *   High | High | no edge     | any  | hold  | hold
     */
    class LS74 final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-14 terminals of the LS74.
         */
        enum class Terminal : std::uint8_t
        {
            CLR1, ///< Flip-flop 1 asynchronous clear (active Low)
            D1,   ///< Flip-flop 1 data input
            CLK1, ///< Flip-flop 1 clock input (rising edge triggered)
            PRE1, ///< Flip-flop 1 asynchronous preset (active Low)
            Q1,   ///< Flip-flop 1 output
            nQ1,  ///< Flip-flop 1 complementary output (/Q)
            GND,  ///< Ground

            nQ2,  ///< Flip-flop 2 complementary output (/Q)
            Q2,   ///< Flip-flop 2 output
            PRE2, ///< Flip-flop 2 asynchronous preset (active Low)
            CLK2, ///< Flip-flop 2 clock input (rising edge triggered)
            D2,   ///< Flip-flop 2 data input
            CLR2, ///< Flip-flop 2 asynchronous clear (active Low)
            VCC   ///< Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LS74 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an LS74 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit LS74(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("1CLR", "1D", "1CLK", "1PRE", "1Q", "1NQ",
         *             "2CLR", "2D", "2CLK", "2PRE", "2Q", "2NQ", "VCC", "GND").
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

        /**
         * \brief Resets both flip-flops: drives Q=Low, NQ=High on each.
         */
        virtual void reset() override;

    private:
        void onVCC(Voltage v);

        static constexpr float k_vccMin = 4.5f; ///< Minimum valid VCC (V)
        static constexpr float k_vccMax = 5.5f; ///< Maximum valid VCC (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        primitive::DFlipFlop m_ff1;
        primitive::DFlipFlop m_ff2;

        signal::Pin m_pin1CLR;
        signal::Pin m_pin1D;
        signal::Pin m_pin1CLK;
        signal::Pin m_pin1PRE;
        signal::Pin m_pin1Q;
        signal::Pin m_pin1NQ;
        signal::Pin m_pinGND;

        signal::Pin m_pin2NQ;
        signal::Pin m_pin2Q;
        signal::Pin m_pin2PRE;
        signal::Pin m_pin2CLK;
        signal::Pin m_pin2D;
        signal::Pin m_pin2CLR;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 14> m_pinPtrs;
    };
} // namespace nfx::silicon::flipflop::sn74xxx
