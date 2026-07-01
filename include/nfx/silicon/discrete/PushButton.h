#pragma once

/**
 * \file PushButton.h
 * \brief Declares the PushButton passive component.
 */

#include "nfx/silicon/component/IDipole.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <span>

namespace nfx::silicon::discrete
{
    /**
     * \class PushButton
     * \brief Two-terminal momentary contact, modeled as a variable resistance.
     */
    class PushButton final : public component::IDipole
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the two terminals of the push button.
         */
        enum class Terminal : std::uint8_t
        {
            A,
            B
        };

        /**
         * \enum ContactType
         * \brief Resting contact behavior of the push button.
         */
        enum class ContactType : std::uint8_t
        {
            NormallyOpen,
            NormallyClosed
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for PushButton construction.
         */
        struct Descriptor
        {
            const char* name;                                     ///< Component name
            ContactType contactType{ ContactType::NormallyOpen }; ///< Resting contact behavior
        };

        /**
         * \brief Constructs a push button from a descriptor.
         * \param descriptor PushButton configuration.
         */
        explicit PushButton(const Descriptor& descriptor);

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
         * \brief Presses the button.
         */
        void press();

        /**
         * \brief Releases the button.
         */
        void release();

        /**
         * \brief Returns whether the button is currently pressed.
         */
        [[nodiscard]] bool isPressed() const;

        /**
         * \brief Returns the contact resistance in ohms.
         *
         * 0.0f when the contact is closed (NormallyOpen + pressed, or
         * NormallyClosed + released), a large finite value otherwise.
         */
        [[nodiscard]] virtual float R() const override;

    private:
        Descriptor m_descriptor;
        bool m_isPressed;
        signal::Pin m_pinA;
        signal::Pin m_pinB;
        std::array<signal::Pin*, 2> m_pinPtrs;
    };
} // namespace nfx::silicon::discrete
