#pragma once

/**
 * \file IDiscrete.h
 * \brief Declares the IDiscrete interface for active discrete components.
 */

#include "nfx/silicon/component/IComponent.h"

namespace nfx::silicon::component
{
    /**
     * \class IDiscrete
     * \brief Interface for active discrete components (Oscillator, ClockDivider, gates).
     *
     * Active discretes are reactive (driven by Pin callbacks).
     */
    class IDiscrete : public IComponent
    {
    public:
        /**
         * \brief Resets the component to its initial state.
         */
        virtual void reset() = 0;

    protected:
        IDiscrete() = default;
    };
} // namespace nfx::silicon::component
