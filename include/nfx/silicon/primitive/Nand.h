#pragma once

/**
 * \file Nand.h
 * \brief Declares the Nand primitive.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::primitive
{
    /**
     * \class Nand
     * \brief Two-input NAND gate.
     *
     * Output Y is driven immediately when any input changes.
     *
     * Pin names follow the TI/Motorola 74xx datasheet convention.
     *
     * Truth table:
     *   A     | B     | Y
     *   ------|-------|------
     *   Low   | Low   | High
     *   Low   | High  | High
     *   High  | Low   | High
     *   High  | High  | Low
     *   HighZ | any   | HighZ
     *   any   | HighZ | HighZ
     */
    class Nand final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the terminals of the Nand gate.
         */
        enum class Terminal : std::uint8_t
        {
            A, ///< Input
            B, ///< Input
            Y  ///< Output
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for Nand construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs a Nand gate from a descriptor.
         * \param descriptor Gate configuration.
         */
        explicit Nand(const Descriptor& descriptor);

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
} // namespace nfx::silicon::primitive
