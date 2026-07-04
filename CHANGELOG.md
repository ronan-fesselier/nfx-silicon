# Changelog

## [Unreleased]

### Added

#### Hardware emulation library (nfx::silicon)

- `nfx::silicon`: C++ library dedicated to hardware emulation
- `Silicon.h`: umbrella header for the full `nfx::silicon` public API
- `Types.h`: shared scalar types (`Level`, `Voltage`)

#### Internal runtime

- `nfx::silicon::internal::runtime::error`: runtime log infrastructure (`Level`, `Kind`, `log`)

##### component

- `IComponent`: base interface for all electronic components (name, pins)
- `IPassive`: interface for passive components
- `IDipole`: interface for two-terminal passives with virtual R()/L()/C()
- `IDiscrete`: interface for active discretes (reset, reactive via Pin callbacks)
- `IInspectable`: orthogonal interface for runtime introspection of internal state

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

##### gate

###### Primitives

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

###### SN74 packages

- `sn74/LS00`: SN74LS00 quad 2-input NAND gate (IPassive), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), four independent Nand gates
- `sn74/LS04`: SN74LS04 hex inverter (IPassive), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), six independent Not gates
- `sn74/LS08`: SN74LS08 quad 2-input AND gate (IPassive), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), four independent And gates
- `sn74/LS32`: SN74LS32 quad 2-input OR gate (IPassive), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), four independent Or gates
- `sn74/LS74`: SN74LS74A dual edge-triggered D flip-flop (IDiscrete), DIP-14 pinout, VCC-gated outputs (HighZ when unpowered), async CLR/PRE active Low, forbidden state releases Q/NQ to HighZ
- `sn74/LS138`: SN74LS138 3-to-8 line decoder/demultiplexer (IPassive), DIP-16 pinout, VCC-gated outputs (HighZ when unpowered), active-Low outputs, enable logic G1·/G2A·/G2B
- `sn74/LS139`: SN74LS139 dual 2-to-4 line decoder/demultiplexer (IPassive), DIP-16 pinout, VCC-gated outputs (HighZ when unpowered), active-Low outputs and enable (/1G, /2G), two independent decoders
- `sn74/LS245`: SN74LS245 octal bus transceiver with 3-STATE outputs (IPassive), DIP-20 pinout, VCC-gated outputs (HighZ when unpowered), bidirectional A/B channels, direction controlled by DIR, isolation via /OE
- `sn74/LS373`: SN74LS373 octal transparent latch (IDiscrete), DIP-20 pinout, VCC-gated outputs (HighZ when unpowered), OC active Low, C (latch enable) High transparent / Low latched, 3-STATE outputs
- `sn74/LS374`: SN74LS374 octal D-type edge-triggered flip-flop (IDiscrete), DIP-20 pinout, VCC-gated outputs (HighZ when unpowered), OC active Low, CLOCK rising edge triggered, 3-STATE outputs

##### signal

- `Signal`: concept and SignalReadType trait for type-safe pin signal access
- `Pin`: single electrical node with digital (Level) and analog (float) signal support
- `Wire`: signal propagation between pins with PushPull, WiredAnd, WiredOr, and Analog resolution models
- `Bus`: named ordered group of wires with type-safe read for digital and analog signals
- `PowerRail`: named voltage reference constants (VDD, VSS, VBB, VEE...)

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

##### gate

- `tests_Gates`: unit tests for Not, And, Or, Nand, Nor, Xor, and Xnor gates (construction, pins, truth table, Terminal enum)
- `tests_SequentialGates`: unit tests for SRLatch, DLatch, DFlipFlop, and JKFlipFlop (construction, pins, truth table, hold, forbidden state, async CLR/PRE, toggle, reset, Terminal enum)
- `tests_74xx`: unit tests for SN74LS74, SN74LS373, SN74LS374, SN74LS04... (construction, DIP pinout, Terminal enum, VCC power gating, OC/CLK/C behavior, async controls, forbidden states, reset, independence)

#### Samples

##### gate

- `half-adder`: half adder sample wiring Xor and And gates via Pin and Wire (S = A ^ B, C = A & B)
- `adder`: full adder sample wiring five gates via Pin and Wire (S = A ^ B ^ Cin, Cout = (A & B) | (Cin & (A ^ B)))
- `register-adder`: scalable N-bit ripple carry adder built from N full adder cells
- `bidirectional-bus-driver`: SN74LS374 datasheet typical application: two LS374 transferring data between two 8-bit buses, clock-gated via LS08, output control inverted via LS04
- `general-register-file`: SN74LS374 datasheet typical application: expandable 4-word by 8-bit general register file, decoded enable and clock via LS139, shared 3-STATE bus with WiredAnd resolution
- `ribbon-cable-transceiver`: SNx4LS245 datasheet typical application: two LS245 as permanently-enabled transceivers across an 8-bit ribbon cable, shared direction control, Master->Slave and Slave->Master transfer, isolation via /OE

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
