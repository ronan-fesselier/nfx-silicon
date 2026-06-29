# Changelog

## [Unreleased]

### Added

#### Hardware emulation library (nfx::silicon)

- `nfx::silicon`: C++ library dedicated to hardware emulation
- `Silicon.h`: umbrella header for the full `nfx::silicon` public API
- `Types.h`: shared scalar types (`Level`, `Voltage`)

##### signal

- `Signal`: concept and SignalReadType trait for type-safe pin signal access
- `Pin`: single electrical node with digital (Level) and analog (float) signal support
- `Wire`: signal propagation between pins with PushPull, WiredAnd, WiredOr, and Analog resolution models

#### Tests

##### signal

- `tests_Pin`: unit tests for Pin drive, read, release, and observer notifications
- `tests_Wire`: unit tests for Wire attach, resolve, and propagation models

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
