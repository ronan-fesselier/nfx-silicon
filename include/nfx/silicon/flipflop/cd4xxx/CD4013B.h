#pragma once

/**
 * \file CD4013B.h
 * \brief Declares the CD4013B CMOS dual D-type flip-flop package.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/primitive/DFlipFlop.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::flipflop::cd4xxx
{
    /**
     * \class CD4013B
     * \brief CD4013B CMOS dual D-type flip-flop: two identical, independent edge-triggered flip-flops in a DIP-14
     * package.
     *
     * CMOS logic family (CD4000 series). Operates from 3V to 18V VDD.
     * Each flip-flop has independent D, CLK, SET, and RESET inputs, plus Q and /Q outputs.
     * SET and RESET are active High (unlike TTL active-Low). Setting or resetting is asynchronous
     * and independent of the clock. Data at D is captured on the rising edge of CLK when both
     * SET and RESET are Low. Forbidden state (SET=High, RESET=High) releases Q and /Q to HighZ.
     * Outputs are gated by VDD: if VDD is not within [3.0V, 18.0V], all outputs are HighZ.
     *
     * Pin names follow the TI CD4013B datasheet (SCHS023E).
     *
     * Pinout (DIP-14):
     *   1  = Q1      14 = VDD
     *   2  = /Q1     13 = Q2
     *   3  = CLOCK1  12 = /Q2
     *   4  = RESET1  11 = CLOCK2
     *   5  = D1      10 = RESET2
     *   6  = SET1     9 = D2
     *   7  = VSS      8 = SET2
     *
     * Truth table (per flip-flop):
     *   SET  | RESET | CLK         | D    | Q     | /Q
     *   -----|-------|-------------|------|-------|------
     *   High | Low   | any         | any  | High  | Low    (async set)
     *   Low  | High  | any         | any  | Low   | High   (async reset)
     *   High | High  | any         | any  | HighZ | HighZ  (forbidden)
     *   Low  | Low   | rising edge | High | High  | Low
     *   Low  | Low   | rising edge | Low  | Low   | High
     *   Low  | Low   | no edge     | any  | hold  | hold
     */
    class CD4013B final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-14 terminals of the CD4013B.
         */
        enum class Terminal : std::uint8_t
        {
            Q1,     ///< pin 1  Flip-flop 1 output
            nQ1,    ///< pin 2  Flip-flop 1 complementary output (/Q)
            CLOCK1, ///< pin 3  Flip-flop 1 clock input (rising edge)
            RESET1, ///< pin 4  Flip-flop 1 asynchronous reset (active High)
            D1,     ///< pin 5  Flip-flop 1 data input
            SET1,   ///< pin 6  Flip-flop 1 asynchronous set (active High)
            VSS,    ///< pin 7  Ground

            SET2,   ///< pin 8  Flip-flop 2 asynchronous set (active High)
            D2,     ///< pin 9  Flip-flop 2 data input
            RESET2, ///< pin 10 Flip-flop 2 asynchronous reset (active High)
            CLOCK2, ///< pin 11 Flip-flop 2 clock input (rising edge)
            nQ2,    ///< pin 12 Flip-flop 2 complementary output (/Q)
            Q2,     ///< pin 13 Flip-flop 2 output
            VDD     ///< pin 14 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CD4013B construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs a CD4013B from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CD4013B(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("Q1", "/Q1", "CLOCK1", "RESET1", "D1", "SET1",
         *             "Q2", "/Q2", "CLOCK2", "RESET2", "D2", "SET2", "VDD", "VSS").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 14 pins in DIP-14 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets both flip-flops: drives Q=Low, /Q=High on each.
         */
        virtual void reset() override;

    private:
        void onVDD(Voltage v);

        static constexpr float k_vddMin = 3.0f;  ///< Minimum valid VDD (V)
        static constexpr float k_vddMax = 18.0f; ///< Maximum valid VDD (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        primitive::DFlipFlop m_ff1;
        primitive::DFlipFlop m_ff2;

        signal::Pin m_pinQ1;
        signal::Pin m_pinNQ1;
        signal::Pin m_pinCLOCK1;
        signal::Pin m_pinRESET1;
        signal::Pin m_pinD1;
        signal::Pin m_pinSET1;
        signal::Pin m_pinVSS;

        signal::Pin m_pinSET2;
        signal::Pin m_pinD2;
        signal::Pin m_pinRESET2;
        signal::Pin m_pinCLOCK2;
        signal::Pin m_pinNQ2;
        signal::Pin m_pinQ2;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 14> m_pinPtrs;
    };
} // namespace nfx::silicon::flipflop::cd4xxx
