#pragma once

/**
 * \file CD4011B.h
 * \brief Declares the CD4011B quad 2-input NAND gate package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/gate/Nand.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::gate::cd4xxx
{
    /**
     * \class CD4011B
     * \brief CD4011B CMOS quad 2-input NAND gate: four independent Nand gates in a DIP-14 package.
     *
     * CMOS logic family (CD4000B series). Operates from 3V to 15V VDD.
     * Encapsulates four Nand gate primitives behind a DIP-14 pin interface.
     * Outputs are gated by VDD: if VDD is not within [3.0V, 15.0V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pin names follow the RCA CD4011B datasheet (92CS-24453/92CS-24763).
     *
     * Pinout (DIP-14):
     *   1  = A     14 = VDD
     *   2  = B     13 = H
     *   3  = J     12 = G
     *   4  = K     11 = M
     *   5  = C     10 = L
     *   6  = D      9 = F
     *   7  = VSS    8 = E
     *
     * Truth table (each gate):
     *   in0   | in1   | out
     *   -------|-------|------
     *   Low   | Low   | High
     *   Low   | High  | High
     *   High  | Low   | High
     *   High  | High  | Low
     *   HighZ | any   | HighZ
     *   any   | HighZ | HighZ
     *
     * Gate mapping:
     *   Gate 1: A(1), B(2)   -> J(3)  = /A·B
     *   Gate 2: C(5), D(6)   -> K(4)  = /C·D
     *   Gate 3: E(8), F(9)   -> L(10) = /E·F
     *   Gate 4: G(12), H(13) -> M(11) = /G·H
     */
    class CD4011B final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-14 terminals of the CD4011B.
         */
        enum class Terminal : std::uint8_t
        {
            A,   ///< pin 1  Gate 1 input
            B,   ///< pin 2  Gate 1 input
            J,   ///< pin 3  Gate 1 output (/A·B)
            K,   ///< pin 4  Gate 2 output (/C·D)
            C,   ///< pin 5  Gate 2 input
            D,   ///< pin 6  Gate 2 input
            VSS, ///< pin 7  Ground

            E,  ///< pin 8  Gate 3 input
            F,  ///< pin 9  Gate 3 input
            L,  ///< pin 10 Gate 3 output (/E·F)
            M,  ///< pin 11 Gate 4 output (/G·H)
            G,  ///< pin 12 Gate 4 input
            H,  ///< pin 13 Gate 4 input
            VDD ///< pin 14 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CD4011B construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U6")
        };

        /**
         * \brief Constructs a CD4011B from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CD4011B(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("A","B","C","D","E","F","G","H","J","K","L","M","VDD","VSS").
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

    private:
        void onVDD(Voltage v);

        static constexpr float k_vddMin = 3.0f;  ///< Minimum valid VDD (V)
        static constexpr float k_vddMax = 15.0f; ///< Maximum valid VDD (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        std::array<Nand, 4> m_gates;

        signal::Pin m_pinA;
        signal::Pin m_pinB;
        signal::Pin m_pinJ;
        signal::Pin m_pinK;
        signal::Pin m_pinC;
        signal::Pin m_pinD;
        signal::Pin m_pinVSS;

        signal::Pin m_pinE;
        signal::Pin m_pinF;
        signal::Pin m_pinL;
        signal::Pin m_pinM;
        signal::Pin m_pinG;
        signal::Pin m_pinH;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 14> m_pinPtrs;
    };
} // namespace nfx::silicon::gate::cd4xxx
