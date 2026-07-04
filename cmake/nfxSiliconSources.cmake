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

    src/discrete/Quartz.cpp

    src/discrete/PowerSupply.cpp

    src/discrete/PushButton.cpp
    src/discrete/Switch.cpp

    # Gate primitives
    src/gate/And.cpp
    src/gate/Nand.cpp
    src/gate/Nor.cpp
    src/gate/Not.cpp
    src/gate/Or.cpp
    src/gate/Xor.cpp
    src/gate/Xnor.cpp

    # Sequential gate primitives
    src/gate/DFlipFlop.cpp
    src/gate/DLatch.cpp
    src/gate/JKFlipFlop.cpp
    src/gate/SRLatch.cpp

    # CD4xxx packages
    src/gate/cd4xxx/CD4011B.cpp

    # 74xx packages
    src/gate/sn74/LS00.cpp
    src/gate/sn74/LS04.cpp
    src/gate/sn74/LS08.cpp
    src/gate/sn74/LS32.cpp
    src/gate/sn74/LS74.cpp
    src/gate/sn74/LS86.cpp
    src/gate/sn74/LS138.cpp
    src/gate/sn74/LS139.cpp
    src/gate/sn74/LS245.cpp
    src/gate/sn74/LS373.cpp
    src/gate/sn74/LS374.cpp

    # Signal layer
    src/signal/Bus.cpp
    src/signal/Pin.cpp
    src/signal/Wire.cpp

    # Internal runtime
    src/internal/runtime/Error.cpp
)
