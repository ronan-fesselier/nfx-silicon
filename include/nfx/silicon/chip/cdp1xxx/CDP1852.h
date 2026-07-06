#pragma once

/**
 * \file CDP1852.h
 * \brief Declares the RCA CDP1852 byte-wide input/output port package.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::chip::cdp1xxx
{
    /**
     * \class CDP1852
     * \brief RCA CDP1852 parallel 8-bit mode-programmable I/O port in a DIP-24 package.
     *
     * MODE=Low configures the device as an input port: a peripheral strobes data
     * into the 8-bit register via CLOCK (High samples, trailing edge latches and
     * sets /SR/SR=Low). When CS1/CS1=High AND CS2=High, DO0..DO7 output the
     * register and /SR/SR returns High.
     *
     * MODE=High configures the device as an output port: the CPU strobes data into
     * the register when CS1/CS1=Low AND CS2=High AND CLOCK=High (trailing edge
     * latches). DO0..DO7 are always driven. SR//SR goes High when deselected,
     * Low on the next CLOCK trailing edge.
     *
     * CLEAR=High resets the register to 0x00 and the SR flip-flop
     * (input mode: /SR/SR=High; output mode: SR//SR=Low).
     *
     * VDD range: 4V-10.5V (CDP1852). Outputs held at HighZ when unpowered.
     *
     * Pin names follow the RCA CDP1852 datasheet (File Number 1166.2, March 1997).
     *
     * Pinout (DIP-24):
     *   1  = nCS1_CS1  24 = VDD
     *   2  = MODE      23 = nSR_SR
     *   3  = DI0       22 = DI7
     *   4  = DO0       21 = DO7
     *   5  = DI1       20 = DI6
     *   6  = DO1       19 = DO6
     *   7  = DI2       18 = DI5
     *   8  = DO2       17 = DO5
     *   9  = DI3       16 = DI4
     *  10  = DO3       15 = DO4
     *  11  = CLOCK     14 = CLEAR
     *  12  = VSS       13 = CS2
     *
     * Truth table (MODE=Low, input port):
     *   CLEAR |  nCS1_CS1  | CS2 | CLOCK | DO
     *   ------|------------|-----|-------|------------------
     *   High  |    X       |  X  |   X   | 0x00 (reset)
     *   Low   |   High     |High |   X   | register (read)
     *   Low   |    X       |  X  | 1->0  | latch DI, nSR_SR=Low
     *   Low   |  !selected |     |       | HighZ, nSR_SR=High
     *
     * Truth table (MODE=High, output port):
     *   CLEAR | nCS1_CS1 | CS2 | CLOCK | DO
     *   ------|----------|-----|-------|------------------
     *   High  |    X     |  X  |   X   | 0x00 (reset)
     *   Low   |   Low    |High | 1->0  | latch DI, drive DO
     *   Low   |    X     |  X  |   X   | register (always driven)
     */
    class CDP1852 final : public component::IDiscrete
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-24 terminals of the CDP1852.
         */
        enum class Terminal : std::uint8_t
        {
            nCS1_CS1, ///< pin 1  Chip select (CS1/CS1: High=selected in input mode, Low=selected in output mode)
            MODE,     ///< pin 2  Mode control (Low=input port, High=output port)
            DI0,      ///< pin 3  Data input bit 0
            DO0,      ///< pin 4  Data output bit 0
            DI1,      ///< pin 5  Data input bit 1
            DO1,      ///< pin 6  Data output bit 1
            DI2,      ///< pin 7  Data input bit 2
            DO2,      ///< pin 8  Data output bit 2
            DI3,      ///< pin 9  Data input bit 3
            DO3,      ///< pin 10 Data output bit 3
            CLOCK,    ///< pin 11 Clock input (data latched on High-to-Low transition)
            VSS,      ///< pin 12 Ground

            CS2,    ///< pin 13 Chip select 2 (active-High)
            CLEAR,  ///< pin 14 Async clear (active-High)
            DO4,    ///< pin 15 Data output bit 4
            DI4,    ///< pin 16 Data input bit 4
            DO5,    ///< pin 17 Data output bit 5
            DI5,    ///< pin 18 Data input bit 5
            DO6,    ///< pin 19 Data output bit 6
            DI6,    ///< pin 20 Data input bit 6
            DO7,    ///< pin 21 Data output bit 7
            DI7,    ///< pin 22 Data input bit 7
            nSR_SR, ///< pin 23 Service request (/SR in input mode: Low=ready; SR in output mode: High=ready)
            VDD     ///< pin 24 Power supply
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CDP1852 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U3")
        };

        /**
         * \brief Constructs a CDP1852 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CDP1852(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("nCS1_CS1", "CS2", "MODE", "CLOCK", "CLEAR", "nSR_SR",
         *             "DI0".."DI7", "DO0".."DO7", "VDD", "VSS").
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
         * \brief Resets the data register to 0x00 and the SR flip-flop to its default state.
         */
        virtual void reset() override;

    private:
        void onVDD(Voltage v);
        void onClock(Level clk);
        void onClear(Level clr);
        void updateOutputs();

        static constexpr float k_vddMin = 4.0f;
        static constexpr float k_vddMax = 10.5f;

        Descriptor m_descriptor;
        bool m_powered{ false };
        std::uint8_t m_register{ 0x00 };
        bool m_sr{ false };
        Level m_lastClock{ Level::HighZ };

        signal::Pin m_pinNCS1_CS1;
        signal::Pin m_pinMODE;
        signal::Pin m_pinDI0;
        signal::Pin m_pinDO0;
        signal::Pin m_pinDI1;
        signal::Pin m_pinDO1;
        signal::Pin m_pinDI2;
        signal::Pin m_pinDO2;
        signal::Pin m_pinDI3;
        signal::Pin m_pinDO3;
        signal::Pin m_pinCLOCK;
        signal::Pin m_pinVSS;

        signal::Pin m_pinCS2;
        signal::Pin m_pinCLEAR;
        signal::Pin m_pinDO4;
        signal::Pin m_pinDI4;
        signal::Pin m_pinDO5;
        signal::Pin m_pinDI5;
        signal::Pin m_pinDO6;
        signal::Pin m_pinDI6;
        signal::Pin m_pinDO7;
        signal::Pin m_pinDI7;
        signal::Pin m_pinNSR_SR;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 24> m_pinPtrs;
        std::array<signal::Pin*, 8> m_diPtrs; ///< DI0..DI7 in order
        std::array<signal::Pin*, 8> m_doPtrs; ///< DO0..DO7 in order
    };
} // namespace nfx::silicon::chip::cdp1xxx
