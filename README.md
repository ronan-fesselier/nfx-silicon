# nfx::silicon

A lightweight, zero-dependency C++20 hardware emulation library.

## Requirements

- C++20 compiler
- CMake 3.25+

## Build

### CMake options

| Option                                    | Default | Description                              |
| ----------------------------------------- | ------- | ---------------------------------------- |
| `NFX_SILICON_BUILD_TESTS`                 | `OFF`   | Build unit tests                         |
| `NFX_SILICON_ENABLE_NATIVE_OPTIMIZATIONS` | `OFF`   | Enable CPU-specific native optimizations |
| `NFX_SILICON_WARNINGS_AS_ERRORS`          | `OFF`   | Treat compiler warnings as errors        |

Example - build everything in debug:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Public API entry point

- `include/nfx/Silicon.h`

## Project Structure

```
nfx-silicon/
├── CMakeLists.txt       # Build entry point
├── cmake/               # CMake modules (sources, target configuration)
├── include/nfx/
│   ├── Silicon.h        # Top-level nfx::silicon API entry point
│   └── silicon/
│       ├── Types.h      # Shared scalar types (Level, Voltage)
│       ├── buffer/      # Buffer packages (CD4049UB, CD4050B, LS245)
│       ├── chip/        # Chip packages (CDP1852, CDP1861)
│       ├── component/   # Component interfaces (IComponent, IPassive, IDipole, IDiscrete, IInspectable)
│       ├── decoder/     # Decoder packages (LS138, LS139)
│       ├── discrete/    # Discrete components (PowerSupply, Resistor, Capacitor, Diode, LED...)
│       ├── flipflop/    # Flip-flop packages (CD4013B, LS74, LS374)
│       ├── gate/        # Gate packages (CD4011B, LS00, LS04, LS08, LS32, LS86)
│       ├── host/        # Host-side abstractions (input)
│       ├── latch/       # Latch packages (CD4042B, LS373)
│       ├── memory/      # Memory packages (CDP1832, I2114)
│       ├── mux/         # Analog mux packages (CD4051B)
│       ├── primitive/   # Logic primitives (And, Or, Nand, Nor, Not, Xor, Xnor, DLatch, DFlipFlop, SRLatch, JKFlipFlop)
│       └── signal/      # Signal layer (Pin, Wire, Bus, PowerRail)
├── samples/             # Runnable samples demonstrating nfx::silicon circuits
├── src/                 # Implementations
└── tests/               # Unit tests
```

## Changelog

See [CHANGELOG.md](CHANGELOG.md).

## License

MIT - see [LICENSE](LICENSE).

## Development dependencies

- [doctest](https://github.com/doctest/doctest) (MIT).
