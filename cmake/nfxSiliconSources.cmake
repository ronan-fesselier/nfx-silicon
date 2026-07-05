#==============================================================================
# nfx::silicon - Sources
#==============================================================================

set(NFX_SILICON_SOURCES
    # Internal runtime
    src/internal/runtime/Error.cpp

    # Signal
    src/signal/Bus.cpp
    src/signal/Pin.cpp
    src/signal/Wire.cpp

    # Discrete
    src/discrete/Capacitor.cpp
    src/discrete/Diode.cpp
    src/discrete/Inductor.cpp
    src/discrete/LED.cpp
    src/discrete/PowerSupply.cpp
    src/discrete/PushButton.cpp
    src/discrete/Quartz.cpp
    src/discrete/Resistor.cpp
    src/discrete/Switch.cpp

    # Primitive
    src/primitive/And.cpp
    src/primitive/Nand.cpp
    src/primitive/Nor.cpp
    src/primitive/Not.cpp
    src/primitive/Or.cpp
    src/primitive/Xor.cpp
    src/primitive/Xnor.cpp

    src/primitive/DFlipFlop.cpp
    src/primitive/DLatch.cpp
    src/primitive/JKFlipFlop.cpp
    src/primitive/SRLatch.cpp

    # Buffer
    src/buffer/cd4xxx/CD4049UB.cpp
    src/buffer/cd4xxx/CD4050B.cpp

    src/buffer/sn74xxx/LS245.cpp
    src/buffer/sn74xxx/LVC1G17.cpp

    # Decoder
    src/decoder/cd4xxx/CD4556B.cpp

    src/decoder/sn74xxx/LS138.cpp
    src/decoder/sn74xxx/LS139.cpp

    # Flip-flop
    src/flipflop/cd4xxx/CD4013B.cpp

    src/flipflop/sn74xxx/LS74.cpp
    src/flipflop/sn74xxx/LS374.cpp

    # Gate
    src/gate/cd4xxx/CD4011B.cpp

    src/gate/sn74xxx/LS00.cpp
    src/gate/sn74xxx/LS04.cpp
    src/gate/sn74xxx/LS08.cpp
    src/gate/sn74xxx/LS32.cpp
    src/gate/sn74xxx/LS86.cpp

    # Latch
    src/latch/cd4xxx/CD4042B.cpp

    src/latch/sn74xxx/LS373.cpp
)
