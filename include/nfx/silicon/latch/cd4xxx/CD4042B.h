#pragma once

/**
 * \file CD4042B.h
 * \brief Declares the CD4042B CMOS quad clocked D latch package.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/primitive/DLatch.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::latch::cd4xxx
{
    /**
     * \class CD4042B
     * \brief CD4042B CMOS quad clocked D latch: four identical latches sharing a common clock and polarity control.
     *
     * CMOS logic family (CD4000 series). Operates from 3V to 18V VDD.
     * Each latch has an independent data input (D) and complementary outputs (Q, /Q).
     * All four latches share a single CLOCK and POLARITY input.
     *
     * Transparency is controlled by the CLOCK level programmed by POLARITY:
     *   POLARITY=0: transparent when CLOCK=Low, latches on rising edge (CLOCK goes High).
     *   POLARITY=1: transparent when CLOCK=High, latches on falling edge (CLOCK goes Low).
     *
     * Internally, the common latch enable signal CL = CLOCK XNOR POLARITY.
     * When CL=High the outputs follow D; when CL=Low the outputs hold.
     * Outputs are gated by VDD: if VDD is not within [3.0V, 18.0V], all outputs are HighZ.
     *
     * Pin names follow the TI CD4042B datasheet (SCHS040D).
     *
     * Pinout (DIP-16):
     *   1  = Q4       16 = VDD
     *   2  = Q1       15 = /Q4
     *   3  = /Q1      14 = D4
     *   4  = D1       13 = D3
     *   5  = CLOCK    12 = /Q3
     *   6  = POLARITY 11 = Q3
     *   7  = D2       10 = Q2
     *   8  = VSS       9 = /Q2
     *
     * Truth table (per latch):
     *   POLARITY | CLOCK | Q
     *   ---------|-------|--------
     *   0        | Low   | D      (transparent)
     *   0        | High  | hold   (latched)
     *   1        | High  | D      (transparent)
     *   1        | Low   | hold   (latched)
     */
    class CD4042B final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-16 terminals of the CD4042B.
         */
        enum class Terminal : std::uint8_t
        {
            Q4,       ///< pin 1  Latch 4 output
            Q1,       ///< pin 2  Latch 1 output
            nQ1,      ///< pin 3  Latch 1 complementary output (/Q)
            D1,       ///< pin 4  Latch 1 data input
            CLOCK,    ///< pin 5  Common clock input
            POLARITY, ///< pin 6  Common polarity control
            D2,       ///< pin 7  Latch 2 data input
            VSS,      ///< pin 8  Ground

            nQ2, ///< pin 9  Latch 2 complementary output (/Q)
            Q2,  ///< pin 10 Latch 2 output
            Q3,  ///< pin 11 Latch 3 output
            nQ3, ///< pin 12 Latch 3 complementary output (/Q)
            D3,  ///< pin 13 Latch 3 data input
            D4,  ///< pin 14 Latch 4 data input
            nQ4, ///< pin 15 Latch 4 complementary output (/Q)
            VDD  ///< pin 16 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CD4042B construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs a CD4042B from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CD4042B(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("Q1".."Q4", "/Q1".."/Q4", "D1".."D4",
         *             "CLOCK", "POLARITY", "VDD", "VSS").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 16 pins in DIP-16 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets all four latches: drives Q=Low, /Q=High on each.
         */
        virtual void reset() override;

    private:
        void onVDD(Voltage v);
        void onClockOrPolarity();

        static constexpr float k_vddMin = 3.0f;  ///< Minimum valid VDD (V)
        static constexpr float k_vddMax = 18.0f; ///< Maximum valid VDD (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        Level m_clock{ Level::HighZ };
        Level m_polarity{ Level::HighZ };

        primitive::DLatch m_latch1;
        primitive::DLatch m_latch2;
        primitive::DLatch m_latch3;
        primitive::DLatch m_latch4;

        signal::Pin m_pinQ4;
        signal::Pin m_pinQ1;
        signal::Pin m_pinNQ1;
        signal::Pin m_pinD1;
        signal::Pin m_pinCLOCK;
        signal::Pin m_pinPOLARITY;
        signal::Pin m_pinD2;
        signal::Pin m_pinVSS;

        signal::Pin m_pinNQ2;
        signal::Pin m_pinQ2;
        signal::Pin m_pinQ3;
        signal::Pin m_pinNQ3;
        signal::Pin m_pinD3;
        signal::Pin m_pinD4;
        signal::Pin m_pinNQ4;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 16> m_pinPtrs;
    };
} // namespace nfx::silicon::latch::cd4xxx
