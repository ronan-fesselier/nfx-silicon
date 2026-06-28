#pragma once

/**
 * \file Signal.h
 * \brief Declares the Signal concept and SignalReadType trait.
 */

#include "nfx/silicon/Types.h"

#include <concepts>

namespace nfx::silicon::signal
{
    /**
     * \concept Signal
     * \brief Constrains template parameters to valid pin signal types.
     */
    template <typename T>
    concept Signal = std::same_as<T, Level> || std::same_as<T, float>;

    /**
     * \struct SignalReadType<Level>
     * \brief Maps a Signal type to its read return type.
     *
     * Level -> Level (HighZ represents not driven).
     */
    template <Signal T>
    struct SignalReadType
    {
        using type = T;
    };

    /**
     * \struct SignalReadType<float>
     * \brief Maps float to Voltage (nullopt represents floating).
     */
    template <>
    struct SignalReadType<float>
    {
        using type = Voltage;
    };
} // namespace nfx::silicon::signal
