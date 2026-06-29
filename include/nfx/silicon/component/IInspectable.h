#pragma once

/**
 * \file IInspectable.h
 * \brief Declares the IInspectable interface for runtime introspection.
 */

#include <cstdint>
#include <optional>
#include <vector>

namespace nfx::silicon::component
{
    /**
     * \struct InspectValue
     * \brief A named internal value exposed for debug and inspection.
     */
    struct InspectValue
    {
        const char* name;    ///< Identifier (e.g. "PC", "D", "DF")
        std::uint32_t value; ///< Current value
        std::uint8_t width;  ///< Width in bits (1, 4, 8, 16, 32)
    };

    /**
     * \class IInspectable
     * \brief Interface for components that expose internal state for inspection.
     *
     * Orthogonal to the component hierarchy. Any IComponent subclass can
     * also inherit IInspectable to expose registers, flags, counters, etc.
     */
    class IInspectable
    {
    public:
        virtual ~IInspectable() = default;

        /**
         * \brief Returns all inspectable internal values.
         */
        [[nodiscard]] virtual std::vector<InspectValue> inspect() const = 0;

        /**
         * \brief Returns a single inspectable value by name.
         * \param name Value identifier.
         */
        [[nodiscard]] virtual std::optional<InspectValue> inspect(const char* name) const = 0;

    protected:
        IInspectable() = default;
    };
} // namespace nfx::silicon::component
