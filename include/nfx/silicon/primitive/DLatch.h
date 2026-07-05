#pragma once

/**
 * \file DLatch.h
 * \brief Declares the DLatch primitive.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::primitive
{
    /**
     * \class DLatch
     * \brief Level-sensitive D latch. Asynchronous, no clock.
     *
     * Transparent when EN is High: Q follows D immediately.
     * Latched when EN is Low: Q holds its last value.
     * Floating input (HighZ on D or EN) holds the current state.
     *
     * Truth table:
     *   EN    | D     | Q    | NQ
     *   ------|-------|------|------
     *   High  | Low   | Low  | High   (transparent)
     *   High  | High  | High | Low    (transparent)
     *   Low   | any   | hold | hold   (latched)
     *   HighZ | any   | hold | hold
     *   any   | HighZ | hold | hold
     */
    class DLatch final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the terminals of the DLatch.
         */
        enum class Terminal : std::uint8_t
        {
            D,  ///< Data input
            EN, ///< Enable input (active High)
            Q,  ///< Output
            NQ  ///< Complementary output (/Q)
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for DLatch construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs a DLatch from a descriptor.
         * \param descriptor Latch configuration.
         */
        explicit DLatch(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name ("D", "EN", "Q", or "NQ").
         * \param name Pin name.
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all pins (D, EN, Q, NQ).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets the latch: Q=Low, NQ=High.
         */
        virtual void reset() override;

    private:
        void compute();

        Descriptor m_descriptor;
        signal::Pin m_pinD;
        signal::Pin m_pinEN;
        signal::Pin m_pinQ;
        signal::Pin m_pinNQ;
        std::array<signal::Pin*, 4> m_pinPtrs;
    };
} // namespace nfx::silicon::primitive
