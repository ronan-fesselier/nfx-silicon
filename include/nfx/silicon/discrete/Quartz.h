#pragma once

/**
 * \file Quartz.h
 * \brief Declares the Quartz passive frequency reference component.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::discrete
{
    /**
     * \class Quartz
     * \brief Passive crystal frequency reference.
     */
    class Quartz final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the two crystal terminals.
         */
        enum class Terminal : std::uint8_t
        {
            X1, ///< Crystal terminal 1
            X2  ///< Crystal terminal 2
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for Quartz construction.
         */
        struct Descriptor
        {
            const char* name;  ///< Component name (e.g. "Y1")
            float frequencyHz; ///< Nominal frequency in hertz
        };

        /**
         * \brief Constructs a Quartz from a descriptor.
         * \param descriptor Crystal configuration.
         */
        explicit Quartz(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name ("X1" or "X2").
         * \param name Pin name.
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all pins (X1, X2).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Returns the nominal frequency in hertz.
         */
        [[nodiscard]] float frequencyHz() const;

        /**
         * \brief Returns the descriptor.
         */
        [[nodiscard]] const Descriptor& descriptor() const;

    private:
        Descriptor m_descriptor;
        signal::Pin m_pinX1;
        signal::Pin m_pinX2;
        std::array<signal::Pin*, 2> m_pinPtrs;
    };
} // namespace nfx::silicon::discrete
