#pragma once

/**
 * \file LS245.h
 * \brief Declares the SN74LS245 octal bus transceiver package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::gate::sn74
{
    /**
     * \class LS245
     * \brief SN74LS245 octal bus transceiver with 3-STATE outputs in a DIP-20 package.
     *
     * Bidirectional 8-channel transceiver. Direction is controlled by DIR;
     * /OE disables all outputs (both sides HighZ).
     *
     * Outputs are gated by VCC: if VCC is not within [4.75V, 5.25V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pinout (DIP-20):
     *   1  = DIR    20 = VCC
     *   2  = A1     19 = /OE
     *   3  = A2     18 = B1
     *   4  = A3     17 = B2
     *   5  = A4     16 = B3
     *   6  = A5     15 = B4
     *   7  = A6     14 = B5
     *   8  = A7     13 = B6
     *   9  = A8     12 = B7
     *  10  = GND    11 = B8
     *
     * Function table (/OE, DIR -> operation):
     *   /OE  | DIR | Operation
     *   -----|-----|----------
     *   Low  | Low | B -> A
     *   Low  | High| A -> B
     *   High |  X  | isolation (all HighZ)
     */
    class LS245 final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-20 terminals of the LS245.
         */
        enum class Terminal : std::uint8_t
        {
            DIR, ///< pin 1  Direction control (Low=B->A, High=A->B)
            A1,  ///< pin 2  Channel 1, A side
            A2,  ///< pin 3  Channel 2, A side
            A3,  ///< pin 4  Channel 3, A side
            A4,  ///< pin 5  Channel 4, A side
            A5,  ///< pin 6  Channel 5, A side
            A6,  ///< pin 7  Channel 6, A side
            A7,  ///< pin 8  Channel 7, A side
            A8,  ///< pin 9  Channel 8, A side
            GND, ///< pin 10 Ground

            B8,  ///< pin 11 Channel 8, B side
            B7,  ///< pin 12 Channel 7, B side
            B6,  ///< pin 13 Channel 6, B side
            B5,  ///< pin 14 Channel 5, B side
            B4,  ///< pin 15 Channel 4, B side
            B3,  ///< pin 16 Channel 3, B side
            B2,  ///< pin 17 Channel 2, B side
            B1,  ///< pin 18 Channel 1, B side
            nOE, ///< pin 19 Active-Low output enable
            VCC  ///< pin 20 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LS245 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an LS245 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit LS245(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("DIR", "A1".."A8", "B1".."B8", "/OE", "VCC", "GND").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 20 pins in DIP-20 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

    private:
        void onVCC(Voltage v);
        void compute();
        void releaseAll();

        static constexpr float k_vccMin = 4.75f;
        static constexpr float k_vccMax = 5.25f;

        Descriptor m_descriptor;
        bool m_powered{ false };
        bool m_computing{ false };
        Level m_lastDir{ Level::HighZ };

        signal::Pin m_pinDIR;
        signal::Pin m_pinA1;
        signal::Pin m_pinA2;
        signal::Pin m_pinA3;
        signal::Pin m_pinA4;
        signal::Pin m_pinA5;
        signal::Pin m_pinA6;
        signal::Pin m_pinA7;
        signal::Pin m_pinA8;
        signal::Pin m_pinGND;

        signal::Pin m_pinB8;
        signal::Pin m_pinB7;
        signal::Pin m_pinB6;
        signal::Pin m_pinB5;
        signal::Pin m_pinB4;
        signal::Pin m_pinB3;
        signal::Pin m_pinB2;
        signal::Pin m_pinB1;
        signal::Pin m_pinNOE;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 20> m_pinPtrs;
    };
} // namespace nfx::silicon::gate::sn74
