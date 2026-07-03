#==============================================================================
# nfx::silicon - Sources
#==============================================================================

set(NFX_SILICON_SOURCES
    # Discrete components
    src/discrete/Capacitor.cpp
    src/discrete/Inductor.cpp
    src/discrete/Resistor.cpp

    src/discrete/Diode.cpp
    src/discrete/LED.cpp

    src/discrete/PowerSupply.cpp

    src/discrete/PushButton.cpp
    src/discrete/Switch.cpp

    # Gate primitives
    src/gate/And.cpp
    src/gate/Nand.cpp
    src/gate/Nor.cpp
    src/gate/Not.cpp
    src/gate/Or.cpp

    # Signal layer
    src/signal/Bus.cpp
    src/signal/Pin.cpp
    src/signal/Wire.cpp

    # Internal runtime
    src/internal/runtime/Error.cpp
)
