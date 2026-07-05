#pragma once

/**
 * \file JKFlipFlop.h
 * \brief Declares the JKFlipFlop primitive.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::primitive
{
    /**
     * \class JKFlipFlop
     * \brief Edge-triggered JK flip-flop with asynchronous clear and preset.
     *
     * Q is updated on the rising edge of CLK via Pin callback.
     * CLR and PRE are asynchronous and override CLK, also via Pin callbacks.
     * Forbidden state (CLR=Low, PRE=Low) releases Q and NQ to HighZ.
     *
     * Truth table:
     *   CLR  | PRE  | CLK         | J    | K    | Q     | NQ
     *   -----|------|-------------|------|------|-------|------
     *   Low  | High | any         | any  | any  | Low   | High   (async clear)
     *   High | Low  | any         | any  | any  | High  | Low    (async preset)
     *   Low  | Low  | any         | any  | any  | HighZ | HighZ  (forbidden)
     *   High | High | rising edge | Low  | Low  | hold  | hold
     *   High | High | rising edge | High | Low  | High  | Low    (set)
     *   High | High | rising edge | Low  | High | Low   | High   (reset)
     *   High | High | rising edge | High | High | /Q    | Q      (toggle)
     *   High | High | no edge     | any  | any  | hold  | hold
     */
    class JKFlipFlop final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the terminals of the JKFlipFlop.
         */
        enum class Terminal : std::uint8_t
        {
            J,   ///< Set input
            K,   ///< Reset input
            CLK, ///< Clock input (rising edge triggered)
            CLR, ///< Asynchronous clear (active Low)
            PRE, ///< Asynchronous preset (active Low)
            Q,   ///< Output
            NQ   ///< Complementary output (/Q)
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for JKFlipFlop construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs a JKFlipFlop from a descriptor.
         * \param descriptor Flip-flop configuration.
         */
        explicit JKFlipFlop(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name ("J", "K", "CLK", "CLR", "PRE", "Q", or "NQ").
         * \param name Pin name.
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all pins (J, K, CLK, CLR, PRE, Q, NQ).
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
        signal::Pin m_pinJ;
        signal::Pin m_pinK;
        signal::Pin m_pinCLK;
        signal::Pin m_pinCLR;
        signal::Pin m_pinPRE;
        signal::Pin m_pinQ;
        signal::Pin m_pinNQ;
        std::array<signal::Pin*, 7> m_pinPtrs;
        Level m_lastCLK{ Level::HighZ };
    };
} // namespace nfx::silicon::primitive
