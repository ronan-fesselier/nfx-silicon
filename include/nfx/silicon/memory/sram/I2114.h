#pragma once

/**
 * \file I2114.h
 * \brief Declares the Intel 2114 1Kx4 static RAM package.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::memory::sram
{
    /**
     * \class I2114
     * \brief Intel 2114 1024x4 static RAM in a DIP-18 package.
     *
     * Ten address inputs (A0..A9) select one of 1024 nibbles. Four
     * bidirectional I/O pins (I/O1..I/O4) carry data in and out.
     * Chip select (/CS) and write enable (/WE) control access.
     *
     * Single +5V supply (TTL, Vcc = 5V +/-5%). Outputs are gated by
     * Vcc: if Vcc is not within [4.75V, 5.25V], all I/O outputs are
     * held at HighZ.
     *
     * Pin names follow the Intel 2114 datasheet.
     *
     * Pinout (DIP-18):
     *   1  = A6     18 = Vcc
     *   2  = A5     17 = A7
     *   3  = A4     16 = A8
     *   4  = A3     15 = A9
     *   5  = A0     14 = I/O1
     *   6  = A1     13 = I/O2
     *   7  = A2     12 = I/O3
     *   8  = /CS    11 = I/O4
     *   9  = GND    10 = /WE
     *
     * Truth table:
     *   /CS | /WE | I/O
     *   ----|-----|------------------
     *   Low | High| ram[addr]   (read)
     *   Low | Low | I/O->ram[addr]   (write)
     *   High| X   | HighZ       (deselected)
     */
    class I2114 final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-18 terminals of the 2114.
         */
        enum class Terminal : std::uint8_t
        {
            A6,  ///< pin 1  Address bit 6
            A5,  ///< pin 2  Address bit 5
            A4,  ///< pin 3  Address bit 4
            A3,  ///< pin 4  Address bit 3
            A0,  ///< pin 5  Address bit 0
            A1,  ///< pin 6  Address bit 1
            A2,  ///< pin 7  Address bit 2
            CS,  ///< pin 8  Chip select (active-Low)
            GND, ///< pin 9  Ground

            WE,  ///< pin 10 Write enable (active-Low)
            IO4, ///< pin 11 Data I/O bit 4
            IO3, ///< pin 12 Data I/O bit 3
            IO2, ///< pin 13 Data I/O bit 2
            IO1, ///< pin 14 Data I/O bit 1
            A9,  ///< pin 15 Address bit 9
            A8,  ///< pin 16 Address bit 8
            A7,  ///< pin 17 Address bit 7
            VCC  ///< pin 18 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for I2114 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U16")
        };

        /**
         * \brief Constructs an I2114 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit I2114(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("A0".."A9", "IO1".."IO4", "CS", "WE", "VCC", "GND").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 18 pins in DIP-18 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets the RAM: clears all stored data to zero.
         */
        virtual void reset() override;

    private:
        void onVCC(Voltage v);
        void onWE(Level lvl);
        void compute();

        static constexpr float k_vccMin = 4.75f;
        static constexpr float k_vccMax = 5.25f;

        Descriptor m_descriptor;
        bool m_powered{ false };
        Level m_wePrev{ Level::High };

        std::array<std::uint8_t, 1024> m_ram{};

        signal::Pin m_pinA6;
        signal::Pin m_pinA5;
        signal::Pin m_pinA4;
        signal::Pin m_pinA3;
        signal::Pin m_pinA0;
        signal::Pin m_pinA1;
        signal::Pin m_pinA2;
        signal::Pin m_pinCS;
        signal::Pin m_pinGND;

        signal::Pin m_pinWE;
        signal::Pin m_pinIO4;
        signal::Pin m_pinIO3;
        signal::Pin m_pinIO2;
        signal::Pin m_pinIO1;
        signal::Pin m_pinA9;
        signal::Pin m_pinA8;
        signal::Pin m_pinA7;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 18> m_pinPtrs;
        std::array<signal::Pin*, 10> m_aPtrs; ///< A0..A9 in order (LSB first)
        std::array<signal::Pin*, 4> m_ioPtrs; ///< I/O1..I/O4 in order
    };
} // namespace nfx::silicon::memory::sram
