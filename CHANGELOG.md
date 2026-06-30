# Changelog

## [Unreleased]

### Added

#### Hardware emulation library (nfx::silicon)

- `nfx::silicon`: C++ library dedicated to hardware emulation
- `Silicon.h`: umbrella header for the full `nfx::silicon` public API
- `Types.h`: shared scalar types (`Level`, `Voltage`)

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
