#pragma once

/**
 * \file Wire.h
 * \brief Declares the Wire class for signal propagation between Pins.
 */

#include "nfx/silicon/signal/Pin.h"

#include <vector>

namespace nfx::silicon::signal
{
    /**
     * \class Wire
     * \brief Connects N pins and propagates signal changes between them.
     *
     * A Wire subscribes to all attached driver pins. When any driver changes,
     * the Wire re-reads all drivers, resolves the net level and propagates
     * it to all attached receiver pins.
     *
     * Wires are non-copyable and non-movable.
     */
    class Wire final
    {
    public:
        /**
         * \enum Resolution
         * \brief Electrical drive topology of the wire.
         */
        enum class Resolution : std::uint8_t
        {
            PushPull, ///< Single driver, asserts on conflict
            WiredAnd, ///< Any Low pulls Low, all HighZ = HighZ
            WiredOr   ///< Any High pulls High, all HighZ = HighZ
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for wire construction.
         */
        struct Descriptor
        {
            const char* name;                              ///< Identifier (e.g. "DATA0", "CLK", "VDD")
            Pin::Kind kind;                                ///< Electrical nature
            Resolution resolution{ Resolution::PushPull }; ///< Signal resolution model
        };

        /**
         * \brief Constructs a wire from a descriptor.
         * \param descriptor Wire configuration.
         */
        explicit Wire(const Descriptor& descriptor);

        ~Wire() = default;

        Wire(const Wire&) = delete;
        Wire& operator=(const Wire&) = delete;
        Wire(Wire&&) = delete;
        Wire& operator=(Wire&&) = delete;

        /**
         * \brief Attaches a pin to this wire.
         * \param pin The pin to connect.
         *
         * Output/Bidirectional/OpenDrain/OpenCollector pins are observed for changes.
         * Input/Bidirectional pins are driven when the wire level changes.
         */
        void attach(Pin& pin);

        /**
         * \brief Reads the current resolved signal value.
         * \return Level (digital) or Voltage (analog).
         */
        template <Signal T>
        [[nodiscard]] typename SignalReadType<T>::type read() const;

        /**
         * \brief Returns the immutable wire configuration.
         */
        [[nodiscard]] const Descriptor& descriptor() const { return m_descriptor; }

    private:
        Descriptor m_descriptor;

        struct Connections
        {
            std::vector<Pin*> drivers;   ///< Pins that can drive (Output, Bidir, OpenDrain, OpenCollector)
            std::vector<Pin*> receivers; ///< Pins that receive (Input, Bidir)
        } m_connections;

        struct State
        {
            Level level{ Level::HighZ };
            Voltage voltage{ std::nullopt };
        } m_state;

        void resolve();
    };

} // namespace nfx::silicon::signal
