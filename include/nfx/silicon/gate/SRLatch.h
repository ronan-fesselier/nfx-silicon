#pragma once

/**
 * \file SRLatch.h
 * \brief Declares the SRLatch gate.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::gate
{
    /**
     * \class SRLatch
     * \brief Set-Reset latch. Asynchronous, no clock.
     *
     * Q and NQ are driven immediately when S or R changes.
     * Forbidden state (S=High, R=High) releases both outputs to HighZ.
     * Floating input (HighZ on S or R) holds the current state.
     *
     * Truth table:
     *   S     | R     | Q    | NQ
     *   ------|-------|------|------
     *   Low   | Low   | hold | hold
     *   High  | Low   | High | Low
     *   Low   | High  | Low  | High
     *   High  | High  | HighZ| HighZ  (forbidden)
     *   HighZ | any   | hold | hold
     *   any   | HighZ | hold | hold
     */
    class SRLatch final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the terminals of the SRLatch.
         */
        enum class Terminal : std::uint8_t
        {
            S, ///< Set input
            R, ///< Reset input
            Q, ///< Output
            NQ ///< Complementary output (/Q)
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for SRLatch construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an SRLatch from a descriptor.
         * \param descriptor Latch configuration.
         */
        explicit SRLatch(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name ("S", "R", "Q", or "NQ").
         * \param name Pin name.
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all pins (S, R, Q, NQ).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets the latch: Q=Low, NQ=High.
         */
        virtual void reset() override;

    private:
        void compute();

        Descriptor m_descriptor;
        signal::Pin m_pinS;
        signal::Pin m_pinR;
        signal::Pin m_pinQ;
        signal::Pin m_pinNQ;
        std::array<signal::Pin*, 4> m_pinPtrs;
    };
} // namespace nfx::silicon::gate
