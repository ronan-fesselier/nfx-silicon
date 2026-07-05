#pragma once

/**
 * \file CD4508B.h
 * \brief Declares the CD4508B dual 4-bit latch package.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::latch::cd4xxx
{
    /**
     * \class CD4508B
     * \brief CD4508B CMOS dual 4-bit latch in a DIP-24 package.
     *
     * Two independent 4-bit latches (A and B), each with data inputs
     * (D0..D3), outputs (Q0..Q3), a STROBE, an OUTPUT DISABLE, and a
     * RESET. Control priority per latch: DISABLE > RESET > STROBE.
     *
     * CMOS logic family (CD4000B series). Operates from 3V to 18V VDD.
     * Outputs are gated by VDD: if VDD is not within [3.0V, 18.0V],
     * all outputs are held at HighZ.
     *
     * Pin names follow the RCA CD4508B datasheet (92CS-27804).
     *
     * Pinout (DIP-24):
     *   1  = RESA    24 = VDD
     *   2  = STRBA   23 = Q3B
     *   3  = DISA    22 = D3B
     *   4  = D0A     21 = Q2B
     *   5  = Q0A     20 = D2B
     *   6  = D1A     19 = Q1B
     *   7  = Q1A     18 = D1B
     *   8  = D2A     17 = Q0B
     *   9  = Q2A     16 = D0B
     *  10  = D3A     15 = DISB
     *  11  = Q3A     14 = STRBB
     *  12  = VSS     13 = RESB
     *
     * Truth table (per latch):
     *   DISABLE | RESET | STROBE | Q
     *   --------|-------|--------|--------
     *   High    | X     | X      | HighZ
     *   Low     | High  | X      | Low
     *   Low     | Low   | High   | D      (transparent)
     *   Low     | Low   | Low    | hold   (latched)
     */
    class CD4508B final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-24 terminals of the CD4508B.
         */
        enum class Terminal : std::uint8_t
        {
            RESA,  ///< pin 1  Latch A reset (active-High)
            STRBA, ///< pin 2  Latch A strobe (High = transparent, Low = latched)
            DISA,  ///< pin 3  Latch A output disable (active-High, forces HighZ)
            D0A,   ///< pin 4  Latch A data input 0
            Q0A,   ///< pin 5  Latch A output 0
            D1A,   ///< pin 6  Latch A data input 1
            Q1A,   ///< pin 7  Latch A output 1
            D2A,   ///< pin 8  Latch A data input 2
            Q2A,   ///< pin 9  Latch A output 2
            D3A,   ///< pin 10 Latch A data input 3
            Q3A,   ///< pin 11 Latch A output 3
            VSS,   ///< pin 12 Ground

            RESB,  ///< pin 13 Latch B reset (active-High)
            STRBB, ///< pin 14 Latch B strobe (High = transparent, Low = latched)
            DISB,  ///< pin 15 Latch B output disable (active-High, forces HighZ)
            D0B,   ///< pin 16 Latch B data input 0
            Q0B,   ///< pin 17 Latch B output 0
            D1B,   ///< pin 18 Latch B data input 1
            Q1B,   ///< pin 19 Latch B output 1
            D2B,   ///< pin 20 Latch B data input 2
            Q2B,   ///< pin 21 Latch B output 2
            D3B,   ///< pin 22 Latch B data input 3
            Q3B,   ///< pin 23 Latch B output 3
            VDD    ///< pin 24 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CD4508B construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U24")
        };

        /**
         * \brief Constructs a CD4508B from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CD4508B(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("RESA", "STRBA", "DISA", "D0A".."D3A",
         *             "Q0A".."Q3A", "RESB", "STRBB", "DISB",
         *             "D0B".."D3B", "Q0B".."Q3B", "VDD", "VSS").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 24 pins in DIP-24 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets both latches: drives all Q outputs Low.
         */
        virtual void reset() override;

    private:
        void onVDD(Voltage v);
        void compute(bool latchB);

        static constexpr float k_vddMin = 3.0f;  ///< Minimum valid VDD (V)
        static constexpr float k_vddMax = 18.0f; ///< Maximum valid VDD (V)

        Descriptor m_descriptor;
        bool m_powered{ false };
        std::uint8_t m_latchA{ 0 }; ///< latched value for latch A
        std::uint8_t m_latchB{ 0 }; ///< latched value for latch B

        signal::Pin m_pinRESA;
        signal::Pin m_pinSTRBA;
        signal::Pin m_pinDISA;
        signal::Pin m_pinD0A;
        signal::Pin m_pinQ0A;
        signal::Pin m_pinD1A;
        signal::Pin m_pinQ1A;
        signal::Pin m_pinD2A;
        signal::Pin m_pinQ2A;
        signal::Pin m_pinD3A;
        signal::Pin m_pinQ3A;
        signal::Pin m_pinVSS;

        signal::Pin m_pinRESB;
        signal::Pin m_pinSTRBB;
        signal::Pin m_pinDISB;
        signal::Pin m_pinD0B;
        signal::Pin m_pinQ0B;
        signal::Pin m_pinD1B;
        signal::Pin m_pinQ1B;
        signal::Pin m_pinD2B;
        signal::Pin m_pinQ2B;
        signal::Pin m_pinD3B;
        signal::Pin m_pinQ3B;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 24> m_pinPtrs;
        std::array<signal::Pin*, 4> m_qAPtrs; ///< Q0A..Q3A in order
        std::array<signal::Pin*, 4> m_dAPtrs; ///< D0A..D3A in order
        std::array<signal::Pin*, 4> m_qBPtrs; ///< Q0B..Q3B in order
        std::array<signal::Pin*, 4> m_dBPtrs; ///< D0B..D3B in order
    };
} // namespace nfx::silicon::latch::cd4xxx
