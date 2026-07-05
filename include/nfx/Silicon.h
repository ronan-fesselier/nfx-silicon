#pragma once

/**
 * \file Silicon.h
 * \brief Umbrella header for nfx::silicon.
 *
 * Include this single header to access the full nfx::silicon public API.
 */

// Core types
#include "nfx/silicon/Types.h"

// Component interfaces
#include "nfx/silicon/component/IComponent.h"
#include "nfx/silicon/component/IDipole.h"
#include "nfx/silicon/component/IDiscrete.h"
#include "nfx/silicon/component/IInspectable.h"
#include "nfx/silicon/component/IPassive.h"

// Discrete components
#include "nfx/silicon/discrete/Capacitor.h"
#include "nfx/silicon/discrete/Inductor.h"
#include "nfx/silicon/discrete/Resistor.h"

#include "nfx/silicon/discrete/Diode.h"
#include "nfx/silicon/discrete/LED.h"

#include "nfx/silicon/discrete/Quartz.h"

#include "nfx/silicon/discrete/PowerSupply.h"

#include "nfx/silicon/discrete/PushButton.h"
#include "nfx/silicon/discrete/Switch.h"

// Gate primitives
#include "nfx/silicon/gate/And.h"
#include "nfx/silicon/gate/Nand.h"
#include "nfx/silicon/gate/Nor.h"
#include "nfx/silicon/gate/Not.h"
#include "nfx/silicon/gate/Or.h"
#include "nfx/silicon/gate/Xor.h"
#include "nfx/silicon/gate/Xnor.h"

// Sequential gate primitives
#include "nfx/silicon/gate/DFlipFlop.h"
#include "nfx/silicon/gate/DLatch.h"
#include "nfx/silicon/gate/JKFlipFlop.h"
#include "nfx/silicon/gate/SRLatch.h"

// CD4xxx packages
#include "nfx/silicon/gate/cd4xxx/CD4011B.h"
#include "nfx/silicon/gate/cd4xxx/CD4049UB.h"
#include "nfx/silicon/gate/cd4xxx/CD4050B.h"

// 74xx packages
#include "nfx/silicon/gate/sn74xxx/LS00.h"
#include "nfx/silicon/gate/sn74xxx/LS04.h"
#include "nfx/silicon/gate/sn74xxx/LS08.h"
#include "nfx/silicon/gate/sn74xxx/LS32.h"
#include "nfx/silicon/gate/sn74xxx/LS74.h"
#include "nfx/silicon/gate/sn74xxx/LS86.h"
#include "nfx/silicon/gate/sn74xxx/LS138.h"
#include "nfx/silicon/gate/sn74xxx/LS245.h"
#include "nfx/silicon/gate/sn74xxx/LS139.h"
#include "nfx/silicon/gate/sn74xxx/LS373.h"
#include "nfx/silicon/gate/sn74xxx/LS374.h"

// Signal layer
#include "nfx/silicon/signal/Bus.h"
#include "nfx/silicon/signal/Pin.h"
#include "nfx/silicon/signal/PowerRail.h"
#include "nfx/silicon/signal/Signal.h"
#include "nfx/silicon/signal/Wire.h"
