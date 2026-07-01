#==============================================================================
# nfx::silicon - Sources
#==============================================================================

set(NFX_SILICON_SOURCES
    # Discrete components
    src/discrete/Capacitor.cpp
    src/discrete/Inductor.cpp
    src/discrete/Resistor.cpp

    src/discrete/Diode.cpp

    src/discrete/PowerSupply.cpp

    src/discrete/PushButton.cpp
    src/discrete/Switch.cpp

    # Signal layer
    src/signal/Bus.cpp
    src/signal/Pin.cpp
    src/signal/Wire.cpp
)
