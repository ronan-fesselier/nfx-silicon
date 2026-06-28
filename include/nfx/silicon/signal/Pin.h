#pragma once

/**
 * \file Pin.h
 * \brief Declares the Pin class and its associated types.
 */

#include "Signal.h"

#include <cstdint>
#include <functional>
#include <type_traits>
#include <vector>

namespace nfx::silicon::signal
{
    /**
     * \class Pin
     * \brief A single electrical node on a component.
     *
     * Pins are non-copyable and non-movable.
     */
    class Pin final
    {
    public:
        /**
         * \enum Kind
         * \brief Electrical nature of the pin.
         */
        enum class Kind : std::uint8_t
        {
            Digital, ///< Standard digital logic
            Analog   ///< Continuous voltage
        };

        /**
         * \enum Direction
         * \brief Signal direction capability.
         */
        enum class Direction : std::uint8_t
        {
            Input,         ///< Receives signals only
            Output,        ///< Drives signals only
            Bidirectional, ///< Can drive or receive
            OpenDrain,     ///< CMOS: drives low or floats
            OpenCollector  ///< BJT: drives low or floats
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for pin construction.
         */
        struct Descriptor
        {
            const char* name;    ///< Identifier (e.g. "D0", "/CE", "CLK")
            Kind kind;           ///< Electrical nature
            Direction direction; ///< Signal direction capability
        };

        /**
         * \brief Constructs a pin from a descriptor.
         * \param descriptor Pin configuration.
         */
        explicit Pin(const Descriptor& descriptor);

        ~Pin() = default;

        Pin(const Pin&) = delete;
        Pin& operator=(const Pin&) = delete;
        Pin(Pin&&) = delete;
        Pin& operator=(Pin&&) = delete;

        /**
         * \brief Drives the pin with a signal value.
         * \param value Level (digital) or float (analog voltage).
         */
        template <Signal T>
        void drive(T value);

        /**
         * \brief Reads the current signal value.
         * \return Level (digital) or std::optional<float> (analog, nullopt = floating).
         */
        template <Signal T>
        [[nodiscard]] typename SignalReadType<T>::type read() const;

        /**
         * \brief Registers a callback invoked when the signal changes or is released.
         * \param callback Function called with the new signal state.
         *
         * For digital pins: callback receives Level (HighZ = released).
         * For analog pins:  callback receives Voltage (nullopt = released).
         */
        template <Signal T>
        void connect(std::function<void(typename SignalReadType<T>::type)> callback);

        /**
         * \brief Releases the pin (HighZ for digital, floating for analog).
         *
         * Notifies observers of the state change.
         */
        void release();

        /**
         * \brief Returns the immutable pin configuration.
         */
        [[nodiscard]] const Descriptor& descriptor() const { return m_descriptor; }

    private:
        Descriptor m_descriptor;

        struct State
        {
            Level level{ Level::HighZ };
            Voltage voltage{ std::nullopt }; ///< nullopt = not driven (floating)
        } m_state;

        struct Observers
        {
            std::vector<std::function<void(Level)>> digitalCallbacks;
            std::vector<std::function<void(Voltage)>> analogCallbacks;
        } m_observers;
    };
} // namespace nfx::silicon::signal
