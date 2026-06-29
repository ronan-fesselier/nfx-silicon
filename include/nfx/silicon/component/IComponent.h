#pragma once

/**
 * \file IComponent.h
 * \brief Declares the IComponent interface, base for all electronic components.
 */

#include "nfx/silicon/signal/Pin.h"

#include <span>

namespace nfx::silicon::component
{
    /**
     * \class IComponent
     * \brief Base interface for any electronic component.
     *
     * Every component has a name and exposes its pins for wiring and inspection.
     * Components are non-copyable and non-movable (pins hold stable addresses).
     */
    class IComponent
    {
    public:
        virtual ~IComponent() = default;

        IComponent(const IComponent&) = delete;
        IComponent& operator=(const IComponent&) = delete;
        IComponent(IComponent&&) = delete;
        IComponent& operator=(IComponent&&) = delete;

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const = 0;

        /**
         * \brief Returns a pin by name.
         * \param name Pin identifier (e.g. "D0", "/CE").
         */
        virtual signal::Pin& pin(const char* name) = 0;

        /**
         * \brief Returns all pins for enumeration and inspection.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const = 0;

    protected:
        IComponent() = default;
    };
} // namespace nfx::silicon::component
