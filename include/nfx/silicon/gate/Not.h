#pragma once

/**
 * \file Not.h
 * \brief Declares the Not gate.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::gate
{
    /**
     * \class Not
     * \brief Single-input inverter gate.
     *
     * Output Y is driven immediately when input A changes.
     *
     * Pin names follow the TI/Motorola 74xx datasheet convention.
     *
     * Truth table:
     *   A     | Y
     *   ------|------
     *   Low   | High
     *   High  | Low
     *   HighZ | HighZ
     */
    class Not final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the terminals of the Not gate.
         */
        enum class Terminal : std::uint8_t
        {
            A, ///< Input
            Y  ///< Output
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for Not construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs a Not gate from a descriptor.
         * \param descriptor Gate configuration.
         */
        explicit Not(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name ("A" or "Y").
         * \param name Pin name.
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all pins (A, Y).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

    private:
        void compute();

        Descriptor m_descriptor;
        signal::Pin m_pinA;
        signal::Pin m_pinY;
        std::array<signal::Pin*, 2> m_pinPtrs;
    };
} // namespace nfx::silicon::gate
