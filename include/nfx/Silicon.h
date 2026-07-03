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

#include "nfx/silicon/discrete/PowerSupply.h"

#include "nfx/silicon/discrete/PushButton.h"
#include "nfx/silicon/discrete/Switch.h"

// Gate primitives
#include "nfx/silicon/gate/And.h"
#include "nfx/silicon/gate/Not.h"
#include "nfx/silicon/gate/Or.h"

// Signal layer
#include "nfx/silicon/signal/Bus.h"
#include "nfx/silicon/signal/Pin.h"
#include "nfx/silicon/signal/PowerRail.h"
#include "nfx/silicon/signal/Signal.h"
#include "nfx/silicon/signal/Wire.h"
