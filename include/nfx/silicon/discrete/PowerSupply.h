#pragma once

/**
 * \file PowerSupply.h
 * \brief Declares the PowerSupply passive component.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/PowerRail.h"

#include <memory>
#include <span>
#include <vector>

namespace nfx::silicon::discrete
{
    /**
     * \class PowerSupply
     * \brief Passive component that drives fixed analog voltages on its output pins.
     *
     * One Analog Output pin is created per rail, named after the rail.
     * Voltages are driven at construction and never change.
     */
    class PowerSupply final : public component::IPassive
    {
    public:
        /**
         * \struct Descriptor
         * \brief Immutable configuration for PowerSupply construction.
         */
        struct Descriptor
        {
            const char* name;                         ///< Component name (e.g. "PSU")
            std::span<const signal::PowerRail> rails; ///< Rails to supply
        };

        /**
         * \brief Constructs a power supply from a descriptor.
         * \param descriptor PowerSupply configuration.
         */
        explicit PowerSupply(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Rail name (e.g. "VDD", "VSS").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns all pins (one per rail).
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

    private:
        Descriptor m_descriptor;
        std::vector<std::unique_ptr<signal::Pin>> m_pins;
        std::vector<signal::Pin*> m_pinPtrs;
    };
} // namespace nfx::silicon::discrete
