#pragma once

/**
 * \file LS373.h
 * \brief Declares the SN74LS373 octal D-type latch package.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/gate/DLatch.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::gate::sn74
{
    /**
     * \class LS373
     * \brief SN74LS373 octal D-type transparent latch with 3-STATE outputs.
     *
     * Encapsulates eight DLatch primitives behind a DIP-20 pin interface.
     * Outputs are gated by VCC: if VCC is not within the valid range [4.75V, 5.25V],
     * all outputs are held at HighZ and inputs are ignored.
     * OC is active Low. When OC is High, all Q outputs go to HighZ regardless
     * of C and D inputs.
     *
     * Pinout (DIP-20):
     *   1  = OC    20 = VCC
     *   2  = 1Q    19 = 8Q
     *   3  = 1D    18 = 8D
     *   4  = 2D    17 = 7D
     *   5  = 2Q    16 = 7Q
     *   6  = 3Q    15 = 6Q
     *   7  = 3D    14 = 6D
     *   8  = 4D    13 = 5D
     *   9  = 4Q    12 = 5Q
     *  10  = GND   11 = C
     *
     * Truth table (per latch):
     *   OC   | C    | Dn    | Qn
     *   -----|------|-------|------
     *   Low  | High | High  | High   (transparent)
     *   Low  | High | Low   | Low    (transparent)
     *   Low  | Low  | any   | hold   (latched)
     *   High | any  | any   | Hi-Z   (output disable)
     */
    class LS373 final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-20 terminals of the LS373.
         */
        enum class Terminal : std::uint8_t
        {
            OC,  ///< Output control (active Low)
            Q1,  ///< Latch 1 non-inverting output
            D1,  ///< Latch 1 data input
            D2,  ///< Latch 2 data input
            Q2,  ///< Latch 2 non-inverting output
            Q3,  ///< Latch 3 non-inverting output
            D3,  ///< Latch 3 data input
            D4,  ///< Latch 4 data input
            Q4,  ///< Latch 4 non-inverting output
            GND, ///< Ground

            C,  ///< Enable input (active High)
            Q5, ///< Latch 5 non-inverting output
            D5, ///< Latch 5 data input
            D6, ///< Latch 6 data input
            Q6, ///< Latch 6 non-inverting output
            Q7, ///< Latch 7 non-inverting output
            D7, ///< Latch 7 data input
            D8, ///< Latch 8 data input
            Q8, ///< Latch 8 non-inverting output
            VCC ///< Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LS373 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs a LS373 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit LS373(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("OC", "C", "1Q".."8Q", "1D".."8D", "VCC", "GND").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        virtual signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 20 pins in DIP-20 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets all eight latches: drives Q=Low on each.
         */
        virtual void reset() override;

    private:
        void onVCC(Voltage v);
        void onOC(Level level);

        static constexpr float k_vccMin = 4.75f; ///< Minimum valid VCC (V)
        static constexpr float k_vccMax = 5.25f; ///< Maximum valid VCC (V)

        Descriptor m_descriptor;
        bool m_powered{ false };
        bool m_outputsEnabled{ false };

        std::array<DLatch, 8> m_latches;

        signal::Pin m_pinOC;
        signal::Pin m_pinQ1;
        signal::Pin m_pinD1;
        signal::Pin m_pinD2;
        signal::Pin m_pinQ2;
        signal::Pin m_pinQ3;
        signal::Pin m_pinD3;
        signal::Pin m_pinD4;
        signal::Pin m_pinQ4;
        signal::Pin m_pinGND;

        signal::Pin m_pinC;
        signal::Pin m_pinQ5;
        signal::Pin m_pinD5;
        signal::Pin m_pinD6;
        signal::Pin m_pinQ6;
        signal::Pin m_pinQ7;
        signal::Pin m_pinD7;
        signal::Pin m_pinD8;
        signal::Pin m_pinQ8;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 20> m_pinPtrs;
    };
} // namespace nfx::silicon::gate::sn74
