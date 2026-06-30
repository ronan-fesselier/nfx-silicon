#pragma once

/**
 * \file Switch.h
 * \brief Declares the Switch passive component.
 */

#include "nfx/silicon/component/IDipole.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <span>

namespace nfx::silicon::discrete
{
    /**
     * \class Switch
     * \brief Two-terminal SPST toggle switch, modeled as a variable resistance.
     */
    class Switch final : public component::IDipole
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the two terminals of the switch.
         */
        enum class Terminal : std::uint8_t
        {
            A,
            B
        };

        /**
         * \enum State
         * \brief Contact state of the switch.
         */
        enum class State : std::uint8_t
        {
            Open,
            Closed
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for Switch construction.
         */
        struct Descriptor
        {
            const char* name;                  ///< Component name
            State initialState{ State::Open }; ///< Contact state at construction
        };

        /**
         * \brief Constructs a switch from a descriptor.
         * \param descriptor Switch configuration.
         */
        explicit Switch(const Descriptor& descriptor);

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
         * \brief Closes the contact.
         */
        void close();

        /**
         * \brief Opens the contact.
         */
        void open();

        /**
         * \brief Returns the current contact state.
         */
        [[nodiscard]] State state() const;

        /**
         * \brief Returns the contact resistance in ohms.
         *
         * 0.0f when Closed, a large finite value when Open.
         */
        [[nodiscard]] virtual float R() const override;

    private:
        Descriptor m_descriptor;
        State m_state;
        signal::Pin m_pinA;
        signal::Pin m_pinB;
        std::array<signal::Pin*, 2> m_pinPtrs;
    };
} // namespace nfx::silicon::discrete
