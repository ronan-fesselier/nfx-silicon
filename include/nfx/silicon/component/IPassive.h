#pragma once

/**
 * \file IPassive.h
 * \brief Declares the IPassive interface for passive electronic components.
 */

#include "nfx/silicon/component/IComponent.h"

namespace nfx::silicon::component
{
    /**
     * \class IPassive
     * \brief Interface for passive components.
     */
    class IPassive : public IComponent
    {
    protected:
        IPassive() = default;
    };
} // namespace nfx::silicon::component
