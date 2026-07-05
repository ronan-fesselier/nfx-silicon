# Changelog

## [Unreleased]

### Added

#### Hardware emulation library (nfx::silicon)

- `nfx::silicon`: C++ library dedicated to hardware emulation
- `Silicon.h`: umbrella header for the full `nfx::silicon` public API
- `Types.h`: shared scalar types (`Level`, `Voltage`)

#### Internal runtime

- `nfx::silicon::internal::runtime::error`: runtime log infrastructure (`Level`, `Kind`, `log`)

#### Components

##### interfaces

- `IComponent`: base interface for all electronic components (name, pins)
- `IPassive`: interface for passive components
- `IDipole`: interface for two-terminal passives with virtual R()/L()/C()
- `IDiscrete`: interface for active discretes (reset, reactive via Pin callbacks)
- `IInspectable`: orthogonal interface for runtime introspection of internal state

##### signal

- `Signal`: concept and SignalReadType trait for type-safe pin signal access
- `Pin`: single electrical node with digital (Level) and analog (float) signal support
- `Wire`: signal propagation between pins with PushPull, WiredAnd, WiredOr, and Analog resolution models
- `Bus`: named ordered group of wires with type-safe read for digital and analog signals
- `PowerRail`: named voltage reference constants (VDD, VSS, VBB, VEE...)

##### discrete

- `PowerSupply`: passive component driving fixed analog voltages per rail
- `Resistor`: two-terminal passive component with a fixed resistance
- `Inductor`: two-terminal passive component with a fixed inductance
- `Capacitor`: two-terminal passive component with a fixed capacitance
- `Diode`: two-terminal passive component with asymmetric conduction (Anode/Cathode)
- `Switch`: two-terminal SPST toggle switch modeled as a variable contact resistance
- `PushButton`: two-terminal momentary contact (NormallyOpen/NormallyClosed) modeled as a variable contact resistance
- `LED`: two-terminal indicator with Color presets and resolved forward voltage
- `Quartz`: passive crystal frequency reference (IPassive), two Analog Bidirectional terminals (X1, X2), frequencyHz stored in descriptor

##### primitive

- `Not`: single-input inverter gate, reactive via Pin callback, truth table Low/High/HighZ
- `And`: two-input AND gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Or`: two-input OR gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Nand`: two-input NAND gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Nor`: two-input NOR gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Xor`: two-input XOR gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Xnor`: two-input XNOR gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `SRLatch`: asynchronous SR latch (IDiscrete), reactive via Pin callbacks, forbidden state releases Q/NQ to HighZ, HighZ input holds state
- `DLatch`: level-sensitive D latch (IDiscrete), transparent when EN=High, holds state when EN=Low or HighZ
- `DFlipFlop`: edge-triggered D flip-flop (IDiscrete), CLK rising edge detected via Pin callback, async CLR/PRE via Pin callbacks, forbidden state releases Q/NQ to HighZ
- `JKFlipFlop`: edge-triggered JK flip-flop (IDiscrete), CLK rising edge detected via Pin callback, async CLR/PRE via Pin callbacks, toggle on J=High K=High, forbidden state releases Q/NQ to HighZ

##### buffer

- `CD4049UB`: CD4049UB CMOS hex inverting buffer (IPassive), DIP-16 pinout, VCC-gated outputs (HighZ when unpowered), six independent Not gates, pin names per TI datasheet (A/B/C/D/E/F inputs, G/H/I/J/K/L outputs), 3V-18V supply range
- `CD4050B`: CD4050B CMOS hex non-inverting buffer (IPassive), DIP-16 pinout, VCC-gated outputs (HighZ when unpowered), six independent pass-through buffers, same pinout as CD4049UB, 3V-18V supply range

- `LS245`: SN74LS245 octal bus transceiver with 3-STATE outputs (IPassive), DIP-20 pinout, VCC-gated outputs (HighZ when unpowered), bidirectional A/B channels, direction controlled by DIR, isolation via /OE

##### decoder

- `LS138`: SN74LS138 3-to-8 line decoder/demultiplexer (IPassive), DIP-16 pinout, VCC-gated outputs (HighZ when unpowered), active-Low outputs, enable logic G1·/G2A·/G2B
- `LS139`: SN74LS139 dual 2-to-4 line decoder/demultiplexer (IPassive), DIP-16 pinout, VCC-gated outputs (HighZ when unpowered), active-Low outputs and enable (/1G, /2G), two independent decoders

##### flipflop

- `CD4013B`: CD4013B CMOS dual D-type flip-flop (IDiscrete), DIP-14 pinout, VDD-gated outputs (HighZ when unpowered), two independent edge-triggered flip-flops, SET/RESET active High (async), forbidden state (SET=High RESET=High) releases Q//Q to HighZ, pin names per TI datasheet (SCHS023E), 3V-18V supply range

- `LS74`: SN74LS74A dual edge-triggered D flip-flop (IDiscrete), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), async CLR/PRE active Low, forbidden state releases Q/NQ to HighZ
- `LS374`: SN74LS374 octal D-type edge-triggered flip-flop (IDiscrete), DIP-20 pinout, VCC-gated outputs (HighZ when unpowered), OC active Low, CLOCK rising edge triggered, 3-STATE outputs

##### gate

- `CD4011B`: CD4011B CMOS quad 2-input NAND gate (IPassive), DIP-14 pinout, VDD-gated outputs (HighZ when unpowered), four independent Nand gates, pin names per RCA datasheet (A/B/C/D/E/F/G/H/J/K/L/M), 3V-15V supply range

- `LS00`: SN74LS00 quad 2-input NAND gate (IPassive), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), four independent Nand gates
- `LS04`: SN74LS04 hex inverter (IPassive), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), six independent Not gates
- `LS08`: SN74LS08 quad 2-input AND gate (IPassive), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), four independent And gates
- `LS32`: SN74LS32 quad 2-input OR gate (IPassive), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), four independent Or gates
- `LS86`: SN74LS86A quad 2-input XOR gate (IPassive), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), four independent Xor gates

##### latch

- `CD4042B`: CD4042B CMOS quad clocked D latch (IDiscrete), DIP-16 pinout, VDD-gated outputs (HighZ when unpowered), four independent latches sharing a common CLOCK and POLARITY, CL=CLOCK XNOR POLARITY enables transparency, pin names per TI datasheet (SCHS040D), 3V-18V supply range

- `LS373`: SN74LS373 octal transparent latch (IDiscrete), DIP-20 pinout, VCC-gated outputs (HighZ when unpowered), OC active Low, C (latch enable) High transparent / Low latched, 3-STATE outputs

#### Tests

##### signal

- `tests_Pin`: unit tests for Pin drive, read, release, and observer notifications
- `tests_Wire`: unit tests for Wire attach, resolve, and propagation models
- `tests_Bus`: unit tests for Bus construction, width, wire access, and read

##### discrete

- `tests_PowerSupply`: unit tests for PowerSupply construction, pin creation, and voltage output
- `tests_Dipoles`: unit tests for Resistor, Capacitor, and Inductor construction, pins, and R()/L()/C() values
- `tests_Diode`: unit tests for Diode construction, pins, Terminal mapping, and forward/Shockley parameters
- `tests_Switches`: unit tests for Switch and PushButton construction, pins, contact state, and R() values
- `tests_LED`: unit tests for LED construction, pins, Terminal mapping, Color presets, and Shockley parameters
- `tests_Quartz`: unit tests for Quartz construction, pins, Terminal mapping, frequencyHz, descriptor, and name

##### primitive

- `tests_Primitives`: unit tests for Not, And, Or, Nand, Nor, Xor, and Xnor gates (construction, pins, truth table, Terminal enum)
- `tests_SequentialPrimitives`: unit tests for SRLatch, DLatch, DFlipFlop, and JKFlipFlop (construction, pins, truth table, hold, forbidden state, async CLR/PRE, toggle, reset, Terminal enum)

##### buffer

- `tests_cd4xxx`: unit tests for CD4049UB and CD4050B (construction, DIP pin names, Terminal enum, truth table, power gating, VCC removal and below-range, buffer independence, component name)
- `tests_sn74xxx`: unit tests for LS245 (construction, DIP pin names, Terminal enum, power gating, DIR/OE control, bidirectional transfer, bus isolation, component name)

##### decoder

- `tests_sn74xxx`: unit tests for LS138 and LS139 (construction, DIP pin names, Terminal enum, power gating, enable logic, output truth table, decoder independence, component name)

##### flipflop

- `tests_cd4xxx`: unit tests for CD4013B (construction, DIP pin names, Terminal enum, power gating, clock capture, async SET/RESET, forbidden state, flip-flop independence, reset(), component name)
- `tests_sn74xxx`: unit tests for LS74 and LS374 (construction, DIP pin names, Terminal enum, power gating, clock capture, async CLR/PRE, forbidden state, OC control, reset(), component name)

##### gate

- `tests_cd4xxx`: unit tests for CD4011B (construction, DIP pin names, Terminal enum, truth table, power gating, VDD removal and below-range, gate independence, component name)
- `tests_sn74xxx`: unit tests for LS00, LS04, LS08, LS32, and LS86 (construction, DIP pin names, Terminal enum, truth table, power gating, VCC removal, gate independence, component name)

##### latch

- `tests_cd4xxx`: unit tests for CD4042B (construction, DIP pin names, Terminal enum, power gating, CLOCK/POLARITY truth table, latch independence, reset(), component name)
- `tests_sn74xxx`: unit tests for LS373 (construction, DIP pin names, Terminal enum, power gating, latch enable, OC control, 3-STATE outputs, reset(), component name)

#### Samples

##### buffer

- `ribbon-cable-transceiver`: SNx4LS245 datasheet typical application: two LS245 as permanently-enabled transceivers across an 8-bit ribbon cable, shared direction control, Master->Slave and Slave->Master transfer, isolation via /OE

##### flipflop

- `bidirectional-bus-driver`: SN74LS374 datasheet typical application: two LS374 transferring data between two 8-bit buses, clock-gated via LS08, output control inverted via LS04
- `general-register-file`: SN74LS374 datasheet typical application: expandable 4-word by 8-bit general register file, decoded enable and clock via LS139, shared 3-STATE bus with WiredAnd resolution

##### primitive

- `half-adder`: half adder sample wiring Xor and And gates via Pin and Wire (S = A ^ B, C = A & B)
- `adder`: full adder sample wiring five gates via Pin and Wire (S = A ^ B ^ Cin, Cout = (A & B) | (Cin & (A ^ B)))
- `register-adder`: scalable N-bit ripple carry adder built from N full adder cells

### Changed

- NIL

### Deprecated

- NIL

### Removed

- NIL

### Fixed

- NIL

### Security

- NIL
