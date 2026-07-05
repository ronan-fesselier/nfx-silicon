#pragma once

/**
 * \file CD4051B.h
 * \brief Declares the CD4051B single 8-channel analog multiplexer/demultiplexer.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <optional>
#include <span>

namespace nfx::silicon::mux::cd4xxx
{
    /**
     * \class CD4051B
     * \brief CD4051B single 8-channel analog multiplexer/demultiplexer in a DIP-16 package.
     *
     * One common terminal (COM) connects to one of eight channels (CH0..CH7),
     * selected by a 3-bit binary address (C, B, A). INH High isolates all channels.
     *
     * CMOS logic family (CD4000B series). Operates from 3V to 18V VDD.
     * Outputs are gated by VDD: if VDD is not within [3.0V, 18.0V],
     * activeChannel() returns nullopt and inputs are ignored.
     *
     * Pin names follow the TI/RCA CD4051B datasheet (SCHS047).
     *
     * Pinout (DIP-16):
     *   1  = CH4   16 = VDD
     *   2  = CH6   15 = CH2
     *   3  = COM   14 = CH1
     *   4  = CH7   13 = CH0
     *   5  = CH5   12 = CH3
     *   6  = INH   11 = A
     *   7  = VEE   10 = B
     *   8  = VSS    9 = C
     *
     * Truth table:
     *   INH | C | B | A | Active channel
     *   ----|---|---|---|---------------
     *   Low | 0 | 0 | 0 | CH0
     *   Low | 0 | 0 | 1 | CH1
     *   Low | 0 | 1 | 0 | CH2
     *   Low | 0 | 1 | 1 | CH3
     *   Low | 1 | 0 | 0 | CH4
     *   Low | 1 | 0 | 1 | CH5
     *   Low | 1 | 1 | 0 | CH6
     *   Low | 1 | 1 | 1 | CH7
     *   High| X | X | X | none
     */
    class CD4051B final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-16 terminals of the CD4051B.
         */
        enum class Terminal : std::uint8_t
        {
            CH4, ///< pin 1  Channel 4 I/O
            CH6, ///< pin 2  Channel 6 I/O
            COM, ///< pin 3  Common I/O
            CH7, ///< pin 4  Channel 7 I/O
            CH5, ///< pin 5  Channel 5 I/O
            INH, ///< pin 6  Inhibit (active High)
            VEE, ///< pin 7  Negative supply
            VSS, ///< pin 8  Ground

            C,   ///< pin 9  Address input C (MSB)
            B,   ///< pin 10 Address input B
            A,   ///< pin 11 Address input A (LSB)
            CH3, ///< pin 12 Channel 3 I/O
            CH0, ///< pin 13 Channel 0 I/O
            CH1, ///< pin 14 Channel 1 I/O
            CH2, ///< pin 15 Channel 2 I/O
            VDD  ///< pin 16 Positive supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CD4051B construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U11")
        };

        /**
         * \brief Constructs a CD4051B from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CD4051B(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("COM", "CH0".."CH7", "A", "B", "C",
         *             "INH", "VDD", "VSS", "VEE").
         */
        signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 16 pins in DIP-16 order.
         */
        [[nodiscard]] std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Returns the active channel index (0-7), or nullopt when
         *        INH is High, any address input is HighZ, or VDD is out of range.
         */
        [[nodiscard]] std::optional<std::uint8_t> activeChannel() const;

    private:
        void onVDD(Voltage v);

        static constexpr float k_vddMin = 3.0f;  ///< Minimum valid VDD-VSS (V)
        static constexpr float k_vddMax = 18.0f; ///< Maximum valid VDD-VSS (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        signal::Pin m_pinCH4;
        signal::Pin m_pinCH6;
        signal::Pin m_pinCOM;
        signal::Pin m_pinCH7;
        signal::Pin m_pinCH5;
        signal::Pin m_pinINH;
        signal::Pin m_pinVEE;
        signal::Pin m_pinVSS;

        signal::Pin m_pinC;
        signal::Pin m_pinB;
        signal::Pin m_pinA;
        signal::Pin m_pinCH3;
        signal::Pin m_pinCH0;
        signal::Pin m_pinCH1;
        signal::Pin m_pinCH2;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 16> m_pinPtrs;
    };
} // namespace nfx::silicon::mux::cd4xxx
