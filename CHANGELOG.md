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

##### gate

- `Not`: single-input inverter gate, reactive via Pin callback, truth table Low/High/HighZ
- `And`: two-input AND gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Or`: two-input OR gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Nand`: two-input NAND gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Nor`: two-input NOR gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Xor`: two-input XOR gate, reactive via Pin callbacks, truth table Low/High/HighZ
- `Xnor`: two-input XNOR gate, reactive via Pin callbacks, truth table Low/High/HighZ

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

##### gate

- `tests_Gates`: unit tests for Not, And, Or, Nand, Nor, Xor, and Xnor gates (construction, pins, truth table, Terminal enum)

#### Samples

##### gate

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
