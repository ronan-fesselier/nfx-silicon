#pragma once

/**
 * \file DFlipFlop.h
 * \brief Declares the DFlipFlop gate.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::gate
{
    /**
     * \class DFlipFlop
     * \brief Edge-triggered D flip-flop with asynchronous clear and preset.
     *
     * Q is sampled from D on the rising edge of CLK via Pin callback.
     * CLR and PRE are asynchronous and override CLK, also via Pin callbacks.
     * Forbidden state (CLR=Low, PRE=Low) releases Q and NQ to HighZ.
     *
     * Truth table:
     *   CLR  | PRE  | CLK         | D    | Q     | NQ
     *   -----|------|-------------|------|-------|------
     *   Low  | High | any         | any  | Low   | High   (async clear)
     *   High | Low  | any         | any  | High  | Low    (async preset)
     *   Low  | Low  | any         | any  | HighZ | HighZ  (forbidden)
     *   High | High | rising edge | Low  | Low   | High
     *   High | High | rising edge | High | High  | Low
     *   High | High | no edge     | any  | hold  | hold
     */
    class DFlipFlop final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the terminals of the DFlipFlop.
         */
        enum class Terminal : std::uint8_t
        {
            D,   ///< Data input
            CLK, ///< Clock input (rising edge triggered)
            CLR, ///< Asynchronous clear (active Low)
            PRE, ///< Asynchronous preset (active Low)
            Q,   ///< Output
            NQ   ///< Complementary output (/Q)
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for DFlipFlop construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs a DFlipFlop from a descriptor.
         * \param descriptor Flip-flop configuration.
         */
        explicit DFlipFlop(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name ("D", "CLK", "CLR", "PRE", "Q", or "NQ").
         * \param name Pin name.
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all pins (D, CLK, CLR, PRE, Q, NQ).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets the flip-flop: drives Q=Low, NQ=High.
         */
        virtual void reset() override;

    private:
        void onCLK(Level level);
        void applyAsync();

        Descriptor m_descriptor;
        signal::Pin m_pinD;
        signal::Pin m_pinCLK;
        signal::Pin m_pinCLR;
        signal::Pin m_pinPRE;
        signal::Pin m_pinQ;
        signal::Pin m_pinNQ;
        std::array<signal::Pin*, 6> m_pinPtrs;
        Level m_lastCLK{ Level::HighZ };
    };
} // namespace nfx::silicon::gate
