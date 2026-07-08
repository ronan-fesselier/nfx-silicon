#pragma once

/**
 * \file CDP1802.h
 * \brief Declares the RCA CDP1802 8-bit microprocessor package.
 */

#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/component/IInspectable.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace nfx::silicon::chip::cdp1xxx
{
    /**
     * \class CDP1802
     * \brief RCA CDP1802 8-bit microprocessor in a DIP-40 package.
     *
     * The CDP1802 is a CMOS 8-bit microprocessor with a 16-bit multiplexed
     * address bus, an 8-bit bidirectional data bus, DMA input/output channels,
     * an interrupt line, and four external flag inputs.
     *
     * Machine cycle structure:
     *   8 CLOCKs = 1 machine cycle. TPA pulses once per cycle to strobe the
     *   high-order address byte (MA0..MA7) into an external latch. TPB follows
     *   TPA by approximately 5 clocks; the low-order address byte is then
     *   stable on MA0..MA7. SC1/SC0 encode the current machine state at TPA.
     *
     * Machine state codes (SC1/SC0 at TPA):
     *   SC1=L SC0=L S0: Fetch (instruction fetch from R(P))
     *   SC1=L SC0=H S1: Execute (may span multiple cycles)
     *   SC1=H SC0=L S2: DMA (transfer via R(0))
     *   SC1=H SC0=H S3: Interrupt acknowledge
     *
     * DMA:
     *   /DMAI (active-Low) requests a DMA input cycle. /DMAO (active-Low)
     *   requests a DMA output cycle. Both are sampled between TPB and TPA.
     *   R(0) holds the DMA address and is auto-incremented after each transfer.
     *   Priority: /DMAI > /DMAO > /INT.
     *
     * Interrupt:
     *   /INT (active-Low) sampled between TPB and TPA. On acknowledge (S3),
     *   X and P are saved in T, X is forced to 2, P forced to 1, IE cleared.
     *   R(1) becomes the interrupt service routine program counter.
     *
     * External flags:
     *   /EF1../EF4 (active-Low) are sampled at the start of each S1 cycle.
     *   Tested by B1/BN1..B4/BN4 conditional branch instructions.
     *
     * /CLEAR (active-Low, Schmitt trigger):
     *   Resets I, N, Q to 0, IE to 1, X/P/R(0) to 0. TPA and TPB are
     *   suppressed during reset. The first post-reset cycle is a 9-clock
     *   initialization cycle; CPU then enters S0.
     *
     * /WAIT (active-Low):
     *   Holds the CPU in PAUSE mode, all outputs frozen, no TPA/TPB.
     *
     * I/O:
     *   N0..N2 encode the I/O port number during OUT/INP instructions (N!=0).
     *   /MRD Low = memory read cycle. /MWR Low pulse = memory write cycle.
     *
     * VDD range: 4V-10.5V. VCC: I/O supply, must be <= VDD, typically 5V.
     * Outputs held at HighZ when unpowered.
     *
     * Pin names follow the RCA/Intersil CDP1802 datasheet.
     *
     * Pinout (DIP-40):
     *   1  = CLOCK    40 = VDD
     *   2  = /WAIT    39 = /XTAL
     *   3  = /CLEAR   38 = /DMAI
     *   4  = Q        37 = /DMAO
     *   5  = SC1      36 = /INT
     *   6  = SC0      35 = /MWR
     *   7  = /MRD     34 = TPA
     *   8  = BUS7     33 = TPB
     *   9  = BUS6     32 = MA7
     *  10  = BUS5     31 = MA6
     *  11  = BUS4     30 = MA5
     *  12  = BUS3     29 = MA4
     *  13  = BUS2     28 = MA3
     *  14  = BUS1     27 = MA2
     *  15  = BUS0     26 = MA1
     *  16  = VCC      25 = MA0
     *  17  = N2       24 = /EF1
     *  18  = N1       23 = /EF2
     *  19  = N0       22 = /EF3
     *  20  = VSS      21 = /EF4
     */
    class CDP1802 final : public component::IDiscrete, public component::IInspectable
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the DIP-40 terminals of the CDP1802.
         */
        enum class Terminal : std::uint8_t
        {
            CLOCK,  ///< pin 1  Single-phase clock input
            nWAIT,  ///< pin 2  Pause control, active-Low (holds CPU, freezes outputs, suppresses TPA/TPB)
            nCLEAR, ///< pin 3  Reset control, active-Low, Schmitt trigger (min 150ns pulse)
            Q,      ///< pin 4  Output flip-flop (set by SEQ, reset by REQ)
            SC1,    ///< pin 5  State code bit 1 (valid at TPA rising edge)
            SC0,    ///< pin 6  State code bit 0 (valid at TPA rising edge)
            nMRD,   ///< pin 7  Memory read strobe, active-Low during read cycle
            BUS7,   ///< pin 8  Data bus bit 7 (MSB), bidirectional, 3-state
            BUS6,   ///< pin 9  Data bus bit 6, bidirectional, 3-state
            BUS5,   ///< pin 10 Data bus bit 5, bidirectional, 3-state
            BUS4,   ///< pin 11 Data bus bit 4, bidirectional, 3-state
            BUS3,   ///< pin 12 Data bus bit 3, bidirectional, 3-state
            BUS2,   ///< pin 13 Data bus bit 2, bidirectional, 3-state
            BUS1,   ///< pin 14 Data bus bit 1, bidirectional, 3-state
            BUS0,   ///< pin 15 Data bus bit 0 (LSB), bidirectional, 3-state
            VCC,    ///< pin 16 I/O supply voltage (must be <= VDD, typically 5V)
            N2,     ///< pin 17 I/O control line 2 (port number MSB during OUT/INP)
            N1,     ///< pin 18 I/O control line 1
            N0,     ///< pin 19 I/O control line 0 (port number LSB during OUT/INP)
            VSS,    ///< pin 20 Ground

            nEF4,  ///< pin 21 External flag 4, active-Low, sampled at S1 start
            nEF3,  ///< pin 22 External flag 3, active-Low, sampled at S1 start
            nEF2,  ///< pin 23 External flag 2, active-Low, sampled at S1 start
            nEF1,  ///< pin 24 External flag 1, active-Low, sampled at S1 start
            MA0,   ///< pin 25 Multiplexed address bit 0 (LSB)
            MA1,   ///< pin 26 Multiplexed address bit 1
            MA2,   ///< pin 27 Multiplexed address bit 2
            MA3,   ///< pin 28 Multiplexed address bit 3
            MA4,   ///< pin 29 Multiplexed address bit 4
            MA5,   ///< pin 30 Multiplexed address bit 5
            MA6,   ///< pin 31 Multiplexed address bit 6
            MA7,   ///< pin 32 Multiplexed address bit 7 (MSB)
            TPB,   ///< pin 33 Timing pulse B (low-order address valid, data strobe)
            TPA,   ///< pin 34 Timing pulse A (high-order address strobed into external latch)
            nMWR,  ///< pin 35 Memory write pulse, active-Low (negative pulse after address stabilizes)
            nINT,  ///< pin 36 Interrupt request, active-Low, sampled between TPB and TPA
            nDMAO, ///< pin 37 DMA output request, active-Low, sampled between TPB and TPA
            nDMAI, ///< pin 38 DMA input request, active-Low, sampled between TPB and TPA
            nXTAL, ///< pin 39 Crystal connection (external oscillator reference)
            VDD    ///< pin 40 Core supply voltage (4V-10.5V)
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for CDP1802 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs a CDP1802 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit CDP1802(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("CLOCK", "nWAIT", "nCLEAR", "Q", "SC1", "SC0",
         *             "nMRD", "nMWR", "TPA", "TPB", "BUS0".."BUS7",
         *             "MA0".."MA7", "N0".."N2", "nEF1".."nEF4",
         *             "nDMAI", "nDMAO", "nINT", "nXTAL", "VDD", "VCC", "VSS").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 40 pins in DIP-40 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

        /**
         * \brief Resets all registers (D, DF, R0..RF, P, X, T, IE, Q, I, N) to their
         *        post-/CLEAR state: I=N=Q=0, IE=1, X=P=0, R(0)=0x0000.
         */
        virtual void reset() override;

        /**
         * \brief Returns all inspectable internal values.
         *
         * Exposes: D (8), DF (1), R0..RF (16 each), P (4), X (4), T (8), IE (1), Q (1).
         */
        [[nodiscard]] virtual std::vector<component::InspectValue> inspect() const override;

        /**
         * \brief Returns a single inspectable value by name.
         * \param name "D", "DF", "R0".."RF", "P", "X", "T", "IE", "Q".
         */
        [[nodiscard]] virtual std::optional<component::InspectValue> inspect(const char* name) const override;

    private:
        void onVDD(Voltage v);
        void onClock(Level clk);
        void onNClear(Level lvl);
        void onNWait(Level lvl);
        void onNDMAI(Level lvl);
        void onNDMAO(Level lvl);
        void onNINT(Level lvl);

        void tickClock();
        void beginCycle();
        void endCycle();
        void fetch();
        void execute();
        void dmaIn();
        void dmaOut();
        void interrupt();

        void driveAddress(uint16_t addr, bool highByte);
        void driveBus(uint8_t data);
        uint8_t readBus() const;
        void releaseBus();
        void updateSC();
        void updateQ();
        void updateN(uint8_t n);

        static constexpr float k_vddMin = 4.0f;
        static constexpr float k_vddMax = 10.5f;

        Descriptor m_descriptor;
        bool m_powered{ false };

        struct Registers
        {
            uint8_t D{ 0x00 };            ///< accumulator
            bool DF{ false };             ///< data flag (carry)
            uint8_t B{ 0x00 };            ///< auxiliary register
            std::array<uint16_t, 16> R{}; ///< scratchpad R(0)..R(F)
            uint8_t P{ 0 };               ///< program counter designator
            uint8_t X{ 0 };               ///< data pointer designator
            uint8_t N{ 0 };               ///< low-order instruction digit
            uint8_t I{ 0 };               ///< high-order instruction digit
            uint8_t T{ 0x00 };            ///< interrupt save (X high nibble, P low)
            bool IE{ true };              ///< interrupt enable
            bool Q{ false };              ///< output flip-flop
        } m_registers;

        struct ClockState
        {
            uint8_t count{ 0 };         ///< clock pulse within current machine cycle (0..7)
            Level last{ Level::HighZ }; ///< previous CLOCK level for edge detection
        } m_clock;

        struct ControlState
        {
            bool clearPending{ false }; ///< /CLEAR asserted, suppress TPA/TPB
            bool waitActive{ false };   ///< /WAIT asserted, freeze outputs
        } m_control;

        struct IORequests
        {
            bool dmaIn{ false };     ///< /DMAI sampled Low (pending DMA input)
            bool dmaOut{ false };    ///< /DMAO sampled Low (pending DMA output)
            bool interrupt{ false }; ///< /INT sampled Low (pending interrupt)
            bool efSampled[4]{};     ///< /EF1../EF4 latched at S1 start
        } m_io;

        struct ExecutionState
        {
            enum class State : uint8_t
            {
                Fetch,
                Execute,
                DmaIn,
                DmaOut,
                Interrupt
            };
            State state{ State::Fetch };     ///< current machine cycle type, decided at end of previous cycle
            bool idling{ false };            ///< IDLE instruction repeating until DMA/INT
            bool didWrite{ false };          ///< write instruction drove nMWR Low this cycle
            uint8_t extraExecuteCycles{ 0 }; ///< remaining extra S1 cycles
            uint8_t executePhase{ 0 };       ///< 0 = first Execute cycle, 1 = second

        } m_exec;

        // DIP-40 pins in order
        signal::Pin m_pinCLOCK;
        signal::Pin m_pinNWAIT;
        signal::Pin m_pinNCLEAR;
        signal::Pin m_pinQ;
        signal::Pin m_pinSC1;
        signal::Pin m_pinSC0;
        signal::Pin m_pinNMRD;
        signal::Pin m_pinBUS7;
        signal::Pin m_pinBUS6;
        signal::Pin m_pinBUS5;
        signal::Pin m_pinBUS4;
        signal::Pin m_pinBUS3;
        signal::Pin m_pinBUS2;
        signal::Pin m_pinBUS1;
        signal::Pin m_pinBUS0;
        signal::Pin m_pinVCC;
        signal::Pin m_pinN2;
        signal::Pin m_pinN1;
        signal::Pin m_pinN0;
        signal::Pin m_pinVSS;

        signal::Pin m_pinNEF4;
        signal::Pin m_pinNEF3;
        signal::Pin m_pinNEF2;
        signal::Pin m_pinNEF1;
        signal::Pin m_pinMA0;
        signal::Pin m_pinMA1;
        signal::Pin m_pinMA2;
        signal::Pin m_pinMA3;
        signal::Pin m_pinMA4;
        signal::Pin m_pinMA5;
        signal::Pin m_pinMA6;
        signal::Pin m_pinMA7;
        signal::Pin m_pinTPB;
        signal::Pin m_pinTPA;
        signal::Pin m_pinNMWR;
        signal::Pin m_pinNINT;
        signal::Pin m_pinNDMAO;
        signal::Pin m_pinNDMAI;
        signal::Pin m_pinNXTAL;
        signal::Pin m_pinVDD;

        std::array<signal::Pin*, 40> m_pinPtrs;
        std::array<signal::Pin*, 8> m_busPtrs; ///< BUS0..BUS7 in order
        std::array<signal::Pin*, 8> m_maPtrs;  ///< MA0..MA7 in order
    };
} // namespace nfx::silicon::chip::cdp1xxx
