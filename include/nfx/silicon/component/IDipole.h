#pragma once

/**
 * \file IDipole.h
 * \brief Declares the IDipole interface for two-terminal passive components.
 */

#include "nfx/silicon/component/IPassive.h"

namespace nfx::silicon::component
{
    /**
     * \class IDipole
     * \brief Interface for two-terminal passive components (resistor, capacitor, inductor).
     *
     * Each dipole exposes R, L, C values. An ideal component overrides only its
     * primary value. The others default to zero.
     */
    class IDipole : public IPassive
    {
    public:
        /**
         * \brief Returns the resistance in ohms.
         */
        [[nodiscard]] virtual float R() const { return 0.0f; }

        /**
         * \brief Returns the inductance in henries.
         */
        [[nodiscard]] virtual float L() const { return 0.0f; }

        /**
         * \brief Returns the capacitance in farads.
         */
        [[nodiscard]] virtual float C() const { return 0.0f; }

    protected:
        IDipole() = default;
    };
} // namespace nfx::silicon::component
