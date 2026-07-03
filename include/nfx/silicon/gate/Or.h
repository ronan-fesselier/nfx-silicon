#pragma once

/**
 * \file Or.h
 * \brief Declares the Or gate.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::gate
{
    /**
     * \class Or
     * \brief Two-input OR gate.
     *
     * Output Y is driven immediately when any input changes.
     *
     * Pin names follow the TI/Motorola 74xx datasheet convention.
     *
     * Truth table:
     *   A     | B     | Y
     *   ------|-------|------
     *   Low   | Low   | Low
     *   Low   | High  | High
     *   High  | Low   | High
     *   High  | High  | High
     *   HighZ | any   | HighZ
     *   any   | HighZ | HighZ
     */
    class Or final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the terminals of the Or gate.
         */
        enum class Terminal : std::uint8_t
        {
            A, ///< Input
            B, ///< Input
            Y  ///< Output
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for Or construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an Or gate from a descriptor.
         * \param descriptor Gate configuration.
         */
        explicit Or(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name ("A", "B", or "Y").
         * \param name Pin name.
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all pins (A, B, Y).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

    private:
        void compute();

        Descriptor m_descriptor;
        signal::Pin m_pinA;
        signal::Pin m_pinB;
        signal::Pin m_pinY;
        std::array<signal::Pin*, 3> m_pinPtrs;
    };
} // namespace nfx::silicon::gate
