#pragma once

/**
 * \file CDP1861.h
 * \brief Declares the RCA CDP1861C video display controller package.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::chip::cdp1xxx
{
    /**
     * \class CDP1861
     * \brief RCA CDP1861C video display controller in a DIP-24 package.
     *
     * Generates composite sync and video for a non-interlaced 262-line/60Hz
     * NTSC-compatible display. Designed for use with the CDP1802 COSMAC
     * microprocessor via TPA/TPB timing pulses and DMA output channel.
     *
     * The device maintains a horizontal line counter driven by TPA and a
     * vertical frame counter (262 lines). Display is organized as a bit-mapped
     * window of 128 lines x 8 bytes per line (64 pixels wide).
     *
     * Display enable/disable:
     *   DISP ON High (pulse) sets the internal display flip-flop.
     *   DISP OFF High (pulse) or /RESET Low clears it.
     *   When disabled, /DMAO and /INT are inhibited; counters keep running.
     *
     * DMA sequence per display line:
     *   /DMAO asserted (Low) for 8 machine cycles; each TPB strobe with
     *   SC1=Low and SC0=High loads one byte from DI0..DI7 into the 8-bit
     *   shift register, then shifts out MSB-first on VIDEO at /CLK rate.
     *
     * /INT is asserted 2 lines before the display window (29 cycles before
     * the first /DMAO burst). /EFX is asserted 4 lines before and after
     * the display window.
     *
     * /RESET (Schmitt trigger): resets all counters, inhibits display,
     * drives all control outputs High (VDD). /CLEAR output mirrors the
     * Schmitt trigger output for resetting external devices.
     *
     * VDD range: 4V-6.5V. Outputs held at HighZ when unpowered.
     *
     * Pin names follow the RCA CDP1861C datasheet (92CS-29464R2).
     *
     * Pinout (DIP-24):
     *   1  = nCLK       24 = VDD
     *   2  = nDMAO      23 = nCLEAR
     *   3  = nINT       22 = SC1
     *   4  = TPA        21 = SC0
     *   5  = TPB        20 = DI7
     *   6  = nCOMPSYNC  19 = DI6
     *   7  = VIDEO      18 = DI5
     *   8  = nRESET     17 = DI4
     *   9  = nEFX       16 = DI3
     *  10  = DISPON     15 = DI2
     *  11  = DISPOFF    14 = DI1
     *  12  = VSS        13 = DI0
     *
     * Display control (Table I):
     *   /RESET | DISP ON      | DISP OFF     | State
     *   -------|--------------|--------------|---------------------------
     *   Low    |   Low        |    X         | RESET
     *   Low    |   High       |    X         | INVALID (undefined)
     *   High   |  0->1 pulse  |    X         | DISPLAY ENABLE
     *   High   |   Low        | 0->1 edge    | DISPLAY DISABLE
     *
     * DMA load condition (per TPB strobe):
     *   SC1=Low AND SC0=High AND TPB=High AND DISP ON AND /RESET=High
     *   -> DI0..DI7 loaded into shift register on /CLK High-to-Low
     */
    class CDP1861 final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-24 terminals of the CDP1861C.
         */
        enum class Terminal : std::uint8_t
        {
            nCLK,      ///< pin 1  Clock input, active-Low (shift register clocked on High-to-Low)
            nDMAO,     ///< pin 2  DMA output request, active-Low, open-drain
            nINT,      ///< pin 3  Interrupt request, active-Low, open-drain
            TPA,       ///< pin 4  Timing pulse A from CDP1802 (horizontal line counter clock)
            TPB,       ///< pin 5  Timing pulse B from CDP1802 (data strobe)
            nCOMPSYNC, ///< pin 6  Composite sync output, active-Low
            VIDEO,     ///< pin 7  Video output, active-High (MSB of shift register)
            nRESET,    ///< pin 8  Reset input, active-Low, Schmitt trigger
            nEFX,      ///< pin 9  Display status flag, active-Low (4 lines before/after display window)
            DISPON,    ///< pin 10 Display on input, Low-to-High pulse sets display flip-flop
            DISPOFF,   ///< pin 11 Display off input, Low-to-High transition resets display flip-flop
            VSS,       ///< pin 12 Ground

            DI0,    ///< pin 13 Data input bit 0
            DI1,    ///< pin 14 Data input bit 1
            DI2,    ///< pin 15 Data input bit 2
            DI3,    ///< pin 16 Data input bit 3
            DI4,    ///< pin 17 Data input bit 4
            DI5,    ///< pin 18 Data input bit 5
            DI6,    ///< pin 19 Data input bit 6
            DI7,    ///< pin 20 Data input bit 7
            SC0,    ///< pin 21 State code bit 0 from CDP1802
            SC1,    ///< pin 22 State code bit 1 from CDP1802
            nCLEAR, ///< pin 23 Clear output, active-Low (Schmitt trigger output, resets external devices)
            VDD     ///< pin 24 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CDP1861 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U2")
        };

        /**
         * \brief Constructs a CDP1861 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CDP1861(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("nCLK", "nDMAO", "nINT", "TPA", "TPB",
         *             "nCOMPSYNC", "VIDEO", "nRESET", "nEFX", "DISPON",
         *             "DISPOFF", "DI0".."DI7", "SC0", "SC1", "nCLEAR",
         *             "VDD", "VSS").
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
         * \brief Resets all counters and the display flip-flop to their initial state.
         */
        virtual void reset() override;

    private:
        void onVDD(Voltage v);
        void onReset(Level rst);
        void onDispOn(Level lvl);
        void onDispOff(Level lvl);
        void onTPA(Level tpa);
        void onTPB(Level tpb);
        void onCLK(Level clk);
        void updateOutputs();

        static constexpr float k_vddMin = 4.0f;
        static constexpr float k_vddMax = 6.5f;
        static constexpr uint16_t k_linesPerField = 262;
        static constexpr uint16_t k_cyclesPerLine = 14;
        static constexpr uint16_t k_displayLines = 128;
        static constexpr uint16_t k_intLineOffset = 2; ///< /INT asserted 2 lines before display window
        static constexpr uint16_t k_efxLineOffset = 4; ///< /EFX asserted 4 lines before/after window
        static constexpr uint8_t k_dmaBytes = 8;       ///< bytes per DMA burst

        Descriptor m_descriptor;
        bool m_powered{ false };
        bool m_displayEnabled{ false };
        bool m_inHSync{ false }; ///< nCOMPSYNC asserted (between TPB-after-13th-TPA and 2nd-TPA-after)
        bool m_inDmaWindow{ false };
        uint16_t m_hCounter{ 0 };      ///< TPA count within current line (0..k_cyclesPerLine-1)
        uint16_t m_tpaAfterHSync{ 0 }; ///< TPA count since trailing edge of nCOMPSYNC (for /DMAO anchor)
        uint16_t m_vCounter{ 0 };      ///< vertical line counter (0..261)
        uint8_t m_shiftReg{ 0x00 };
        uint8_t m_shiftBit{ 8 };
        uint8_t m_dmaCount{ 0 };
        Level m_lastTPA{ Level::HighZ };
        Level m_lastTPB{ Level::HighZ };
        bool m_tpbLoadArmed{ false };
        Level m_lastDISPON{ Level::HighZ };
        Level m_lastCLK{ Level::HighZ };

        signal::Pin m_pinNCLK;
        signal::Pin m_pinNDMAO;
        signal::Pin m_pinNINT;
        signal::Pin m_pinTPA;
        signal::Pin m_pinTPB;
        signal::Pin m_pinNCOMPSYNC;
        signal::Pin m_pinVIDEO;
        signal::Pin m_pinNRESET;
        signal::Pin m_pinNEFX;
        signal::Pin m_pinDISPON;
        signal::Pin m_pinDISPOFF;
        signal::Pin m_pinVSS;

        signal::Pin m_pinDI0;
        signal::Pin m_pinDI1;
        signal::Pin m_pinDI2;
        signal::Pin m_pinDI3;
        signal::Pin m_pinDI4;
        signal::Pin m_pinDI5;
        signal::Pin m_pinDI6;
        signal::Pin m_pinDI7;
        signal::Pin m_pinSC0;
        signal::Pin m_pinSC1;
        signal::Pin m_pinNCLEAR;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 24> m_pinPtrs;
        std::array<signal::Pin*, 8> m_diPtrs; ///< DI0..DI7 in order
    };
} // namespace nfx::silicon::chip::cdp1xxx
