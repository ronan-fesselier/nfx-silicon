#pragma once

/**
 * \file CD4050B.h
 * \brief Declares the CD4050B CMOS hex non-inverting buffer package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::buffer::cd4xxx
{
    /**
     * \class CD4050B
     * \brief CD4050B CMOS hex non-inverting buffer: six independent buffers in a DIP-16 package.
     *
     * CMOS logic family (CD4000 series). Operates from 3V to 18V VCC.
     * Encapsulates six buffer stages behind a DIP-16 pin interface.
     * Outputs are gated by VCC: if VCC is not within [3.0V, 18.0V],
     * all outputs are held at HighZ and inputs are ignored.
     *
     * Pin names follow the TI CD4050B datasheet (SCHS046L).
     *
     * Pinout (DIP-16):
     *   1  = VCC    16 = NC
     *   2  = G      15 = L
     *   3  = A      14 = F
     *   4  = H      13 = NC
     *   5  = B      12 = K
     *   6  = I      11 = E
     *   7  = C      10 = J
     *   8  = VSS     9 = D
     *
     * Gate mapping:
     *   Buffer 1: A(3)  -> G(2)  = A
     *   Buffer 2: B(5)  -> H(4)  = B
     *   Buffer 3: C(7)  -> I(6)  = C
     *   Buffer 4: D(9)  -> J(10) = D
     *   Buffer 5: E(11) -> K(12) = E
     *   Buffer 6: F(14) -> L(15) = F
     *
     * Pins 13 and 16 (NC) are not connected internally.
     */
    class CD4050B final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-16 terminals of the CD4050B.
         */
        enum class Terminal : std::uint8_t
        {
            VCC, ///< pin 1  Power supply
            G,   ///< pin 2  Buffer 1 output (=A)
            A,   ///< pin 3  Buffer 1 input
            H,   ///< pin 4  Buffer 2 output (=B)
            B,   ///< pin 5  Buffer 2 input
            I,   ///< pin 6  Buffer 3 output (=C)
            C,   ///< pin 7  Buffer 3 input
            VSS, ///< pin 8  Ground

            D, ///< pin 9  Buffer 4 input
            J, ///< pin 10 Buffer 4 output (=D)
            E, ///< pin 11 Buffer 5 input
            K, ///< pin 12 Buffer 5 output (=E)
            F, ///< pin 14 Buffer 6 input
            L  ///< pin 15 Buffer 6 output (=F)
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CD4050B construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U5")
        };

        /**
         * \brief Constructs a CD4050B from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CD4050B(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("A".."F", "G".."L", "VCC", "VSS").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 14 active pins in DIP-16 order (NC pins excluded).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

    private:
        void onVCC(Voltage v);

        static constexpr float k_vccMin = 3.0f;  ///< Minimum valid VCC (V)
        static constexpr float k_vccMax = 18.0f; ///< Maximum valid VCC (V)

        Descriptor m_descriptor;
        bool m_powered{ false };

        signal::Pin m_pinVCC;
        signal::Pin m_pinG;
        signal::Pin m_pinA;
        signal::Pin m_pinH;
        signal::Pin m_pinB;
        signal::Pin m_pinI;
        signal::Pin m_pinC;
        signal::Pin m_pinVSS;

        signal::Pin m_pinD;
        signal::Pin m_pinJ;
        signal::Pin m_pinE;
        signal::Pin m_pinK;
        signal::Pin m_pinF;
        signal::Pin m_pinL;

        std::array<signal::Pin*, 14> m_pinPtrs;
    };
} // namespace nfx::silicon::buffer::cd4xxx
