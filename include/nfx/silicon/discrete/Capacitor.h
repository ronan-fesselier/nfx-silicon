#pragma once

/**
 * \file Capacitor.h
 * \brief Declares the Capacitor passive component.
 */

#include "nfx/silicon/component/IDipole.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <span>

namespace nfx::silicon::discrete
{
    /**
     * \class Capacitor
     * \brief Two-terminal passive component with a fixed capacitance.
     */
    class Capacitor final : public component::IDipole
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the two terminals of the capacitor.
         */
        enum class Terminal : std::uint8_t
        {
            A,
            B
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for Capacitor construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name
            float farads;     ///< Capacitance value
        };

        /**
         * \brief Constructs a capacitor from a descriptor.
         * \param descriptor Capacitor configuration.
         */
        explicit Capacitor(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name ("A" or "B").
         * \param name Pin name.
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all pins (A, B).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Returns the capacitance in farads.
         */
        [[nodiscard]] virtual float C() const override;

    private:
        Descriptor m_descriptor;
        signal::Pin m_pinA;
        signal::Pin m_pinB;
        std::array<signal::Pin*, 2> m_pinPtrs;
    };
} // namespace nfx::silicon::discrete
