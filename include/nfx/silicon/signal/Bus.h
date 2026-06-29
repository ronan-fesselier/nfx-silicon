#pragma once

/**
 * \file Bus.h
 * \brief Declares the Bus class for multi-wire signal access.
 */

#include "nfx/silicon/signal/Wire.h"

#include <cstdint>
#include <span>

namespace nfx::silicon::signal
{
    /**
     * \class Bus
     * \brief Named ordered group of wires for convenient multi-wire access.
     *
     * A Bus does not own its Wires.
     * Buses are non-copyable and non-movable.
     */
    class Bus final
    {
    public:
        /**
         * \struct Descriptor
         * \brief Immutable configuration for bus construction.
         */
        struct Descriptor
        {
            const char* name;       ///< Identifier (e.g. "DATA", "ADDR")
            Pin::Kind kind;         ///< Electrical nature
            std::span<Wire*> wires; ///< Ordered wires (index 0 = wire 0)
        };

        /**
         * \brief Constructs a bus from a descriptor.
         * \param descriptor Bus configuration.
         */
        explicit Bus(const Descriptor& descriptor);

        ~Bus() = default;

        Bus(const Bus&) = delete;
        Bus& operator=(const Bus&) = delete;
        Bus(Bus&&) = delete;
        Bus& operator=(Bus&&) = delete;

        /**
         * \brief Returns the number of wires in the bus.
         */
        [[nodiscard]] std::size_t width() const;

        /**
         * \brief Returns a reference to the wire at the given index.
         * \param index Wire index (0-based).
         */
        [[nodiscard]] Wire& wire(std::size_t index) const;

        /**
         * \brief Reads all wires into a caller-owned buffer.
         * \param out Span of size width(), filled with resolved signal values.
         */
        template <Signal T>
        void read(std::span<typename SignalReadType<T>::type> out) const;

        /**
         * \brief Returns the immutable bus configuration.
         */
        [[nodiscard]] const Descriptor& descriptor() const { return m_descriptor; }

    private:
        Descriptor m_descriptor;
    };
} // namespace nfx::silicon::signal
