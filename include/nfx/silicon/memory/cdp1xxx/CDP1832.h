#pragma once

/**
 * \file CDP1832.h
 * \brief Declares the CDP1832 mask-programmable ROM package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::memory::cdp1xxx
{
    /**
     * \class CDP1832
     * \brief CDP1832 CMOS 512x8 mask-programmable ROM in a DIP-24 package.
     *
     * Nine address inputs (MA0..MA7, A8) select one of 512 bytes. The 8-bit
     * word is driven on BUS0..BUS7 when CS is Low. When CS is High or VDD is
     * not within range, all BUS outputs are held at HighZ.
     *
     * ROM content is fixed at construction (mask-programmed at fabrication).
     * No write path exists.
     *
     * Pin names follow the RCA CDP1832 datasheet (92CS-27579R2).
     *
     * Pinout (DIP-24):
     *   1  = MA7    24 = VDD
     *   2  = MA6    23 = A8
     *   3  = MA5    22 = NC
     *   4  = MA4    21 = NC
     *   5  = MA3    20 = CS (active-Low)
     *   6  = MA2    19 = NC
     *   7  = MA1    18 = NC
     *   8  = MA0    17 = BUS7
     *   9  = BUS0   16 = BUS6
     *  10  = BUS1   15 = BUS5
     *  11  = BUS2   14 = BUS4
     *  12  = VSS    13 = BUS3
     *
     * Truth table:
     *   CS  | VDD in range | BUS
     *   ----|--------------|------------------
     *   Low | Yes          | rom[MA0..MA7, A8]
     *   High| X            | HighZ
     *   X   | No           | HighZ
     */
    class CDP1832 final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-24 terminals of the CDP1832.
         */
        enum class Terminal : std::uint8_t
        {
            MA7,  ///< pin 1  Address bit 7
            MA6,  ///< pin 2  Address bit 6
            MA5,  ///< pin 3  Address bit 5
            MA4,  ///< pin 4  Address bit 4
            MA3,  ///< pin 5  Address bit 3
            MA2,  ///< pin 6  Address bit 2
            MA1,  ///< pin 7  Address bit 1
            MA0,  ///< pin 8  Address bit 0
            BUS0, ///< pin 9  Data bit 0
            BUS1, ///< pin 10 Data bit 1
            BUS2, ///< pin 11 Data bit 2
            VSS,  ///< pin 12 Ground

            BUS3, ///< pin 13 Data bit 3
            BUS4, ///< pin 14 Data bit 4
            BUS5, ///< pin 15 Data bit 5
            BUS6, ///< pin 16 Data bit 6
            BUS7, ///< pin 17 Data bit 7
            NC18, ///< pin 18 No connection
            NC19, ///< pin 19 No connection
            CS,   ///< pin 20 Chip select (active-Low)
            NC21, ///< pin 21 No connection
            NC22, ///< pin 22 No connection
            A8,   ///< pin 23 Address bit 8
            VDD   ///< pin 24 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CDP1832 construction.
         */
        struct Descriptor
        {
            const char* name;                  ///< Component name (e.g. "U10")
            std::array<std::uint8_t, 512> rom; ///< ROM content (mask-programmed, copied at construction)
        };

        /**
         * \brief Constructs a CDP1832 from a descriptor.
         * \param descriptor Package configuration including ROM content.
         */
        explicit CDP1832(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("MA0".."MA7", "A8", "BUS0".."BUS7", "CS", "VDD", "VSS").
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

    private:
        void onVDD(Voltage v);
        void compute();

        static constexpr float k_vddMin = 4.0f;
        static constexpr float k_vddMax = 10.5f;

        Descriptor m_descriptor;
        bool m_powered{ false };

        signal::Pin m_pinMA7;
        signal::Pin m_pinMA6;
        signal::Pin m_pinMA5;
        signal::Pin m_pinMA4;
        signal::Pin m_pinMA3;
        signal::Pin m_pinMA2;
        signal::Pin m_pinMA1;
        signal::Pin m_pinMA0;
        signal::Pin m_pinBUS0;
        signal::Pin m_pinBUS1;
        signal::Pin m_pinBUS2;
        signal::Pin m_pinVSS;

        signal::Pin m_pinBUS3;
        signal::Pin m_pinBUS4;
        signal::Pin m_pinBUS5;
        signal::Pin m_pinBUS6;
        signal::Pin m_pinBUS7;
        signal::Pin m_pinNC18;
        signal::Pin m_pinNC19;
        signal::Pin m_pinCS;
        signal::Pin m_pinNC21;
        signal::Pin m_pinNC22;
        signal::Pin m_pinA8;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 24> m_pinPtrs;
        std::array<signal::Pin*, 9> m_maPtrs;  ///< MA0..MA7, A8 in order (LSB first)
        std::array<signal::Pin*, 8> m_busPtrs; ///< BUS0..BUS7 in order
    };
} // namespace nfx::silicon::memory::cdp1xxx
