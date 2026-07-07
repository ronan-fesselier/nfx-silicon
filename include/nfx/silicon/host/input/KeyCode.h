#pragma once

/**
 * \file KeyCode.h
 * \brief Declares normalized host-side key codes.
 */

#include <cstddef>
#include <cstdint>
#include <functional>

namespace nfx::silicon::host::input
{
    /**
     * \enum KeyCode
     * \brief Normalized host key codes used by host/input adapters.
     */
    enum class KeyCode : std::uint16_t
    {
        Unknown = 0,

        // Letters
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        // Numbers
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,

        // Special keys
        Space,
        Enter,
        Escape,
        Tab,
        Backspace,

        // Arrow keys
        Left,
        Right,
        Up,
        Down
    };
} // namespace nfx::silicon::host::input

namespace std
{
    template <>
    struct hash<nfx::silicon::host::input::KeyCode>
    {
        std::size_t operator()(const nfx::silicon::host::input::KeyCode key) const noexcept
        {
            return std::hash<std::uint16_t>{}(static_cast<std::uint16_t>(key));
        }
    };
} // namespace std
