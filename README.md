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
├── CMakeLists.txt    # Build entry point
├── cmake/            # CMake modules (sources, target configuration)
├── include/nfx/
│   ├── Silicon.h     # Top-level nfx::silicon API entry point
│   └── silicon/
│       ├── Types.h   # Shared scalar types (Level, Voltage)
│       └── signal/   # Signal layer (Signal, Pin)
├── src/              # Implementations
└── tests/            # Unit tests
```

## Changelog

See [CHANGELOG.md](CHANGELOG.md).

## License

MIT - see [LICENSE](LICENSE).

## Development dependencies

- [doctest](https://github.com/doctest/doctest) (MIT).
