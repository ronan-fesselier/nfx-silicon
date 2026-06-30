#==============================================================================
# nfx::silicon - Sources
#==============================================================================

set(NFX_SILICON_SOURCES
    # Discrete components
    src/discrete/Capacitor.cpp
    src/discrete/Inductor.cpp
    src/discrete/Resistor.cpp

    src/discrete/PowerSupply.cpp

    # Signal layer
    src/signal/Bus.cpp
    src/signal/Pin.cpp
    src/signal/Wire.cpp
)
