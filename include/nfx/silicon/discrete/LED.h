#pragma once

/**
 * \file LED.h
 * \brief Declares the LED passive component.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::discrete
{
    /**
     * \class LED
     * \brief Two-terminal indicator with asymmetric conduction and color-coded forward voltage.
     */
    class LED final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the two terminals of the LED.
         */
        enum class Terminal : std::uint8_t
        {
            Anode,
            Cathode
        };

        /**
         * \enum Color
         * \brief LED color presets with approximate reference forward voltages.
         */
        enum class Color : std::uint8_t
        {
            Red,    ///< ~1.8V
            Green,  ///< ~2.1V
            Blue,   ///< ~3.0V
            Yellow, ///< ~2.0V
            White,  ///< ~3.2V
            Custom  ///< Uses forwardVoltage from Descriptor directly
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LED construction.
         */
        struct Descriptor
        {
            const char* name;                  ///< Component name
            Color color{ Color::Red };         ///< LED color (determines forwardVoltage unless Custom)
            float forwardVoltage{ 0.0f };      ///< Used only when color == Custom
            float saturationCurrent{ 0.0f };   ///< Reverse saturation current (Shockley model)
            float emissionCoefficient{ 1.0f }; ///< Ideality factor (Shockley model)
        };

        /**
         * \brief Constructs a LED from a descriptor.
         * \param descriptor LED configuration.
         */
        explicit LED(const Descriptor& descriptor);

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
         * \brief Returns the resolved forward voltage.
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
        float m_forwardVoltage;
        signal::Pin m_pinAnode;
        signal::Pin m_pinCathode;
        std::array<signal::Pin*, 2> m_pinPtrs;
    };
} // namespace nfx::silicon::discrete
