#pragma once

/**
 * \file KeyEvent.h
 * \brief Declares normalized host key event payload.
 */

#include "KeyCode.h"

namespace nfx::silicon::host::input
{
    /**
     * \struct KeyEvent
     * \brief One normalized host key event.
     */
    struct KeyEvent
    {
        KeyCode key{ KeyCode::Unknown }; // Normalized host key code
        bool pressed{ false };           // True when key is pressed, false when released
        bool repeat{ false };            // True when event originates from key repeat
    };
} // namespace nfx::silicon::host::input
