#pragma once

/**
 * \file Diode.h
 * \brief Declares the Diode passive component.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::discrete
{
    /**
     * \class Diode
     * \brief Two-terminal passive component with asymmetric conduction (Anode -> Cathode).
     */
    class Diode final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the two terminals of the diode.
         */
        enum class Terminal : std::uint8_t
        {
            A, ///< Anode
            K  ///< Cathode
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for Diode construction.
         */
        struct Descriptor
        {
            const char* name;                  ///< Component name
            float forwardVoltage{ 0.0f };      ///< Threshold voltage (simple model)
            float saturationCurrent{ 0.0f };   ///< Reverse saturation current (Shockley model)
            float emissionCoefficient{ 1.0f }; ///< Ideality factor (Shockley model)
        };

        /**
         * \brief Constructs a diode from a descriptor.
         * \param descriptor Diode configuration.
         */
        explicit Diode(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name ("A" or "K").
         * \param name Pin name.
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all pins (Anode, Cathode).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Returns the threshold voltage.
         */
        [[nodiscard]] float forwardVoltage() const;

        /**
         * \brief Returns the reverse saturation current.
         */
        [[nodiscard]] float saturationCurrent() const;

        /**
         * \brief Returns the ideality factor.
         */
        [[nodiscard]] float emissionCoefficient() const;

    private:
        Descriptor m_descriptor;
        signal::Pin m_pinAnode;
        signal::Pin m_pinCathode;
        std::array<signal::Pin*, 2> m_pinPtrs;
    };
} // namespace nfx::silicon::discrete
