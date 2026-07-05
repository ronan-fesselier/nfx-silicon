#pragma once

/**
 * \file LVC1G17.h
 * \brief Declares the SN74LVC1G17 single Schmitt-trigger buffer package.
 */

#include "nfx/silicon/component/IPassive.h"
#include "nfx/silicon/signal/Pin.h"

#include <array>
#include <cstdint>
#include <span>

namespace nfx::silicon::buffer::sn74xxx
{
    /**
     * \class LVC1G17
     * \brief SN74LVC1G17 single Schmitt-trigger buffer in a DBV (SOT-23-5) package.
     *
     * Single non-inverting buffer with Schmitt-trigger input. Output Y follows
     * input A. The Schmitt-trigger characteristic is a physical property of the
     * input stage.
     *
     * Outputs are gated by VCC: if VCC is not within [1.65V, 5.5V],
     * Y is held at HighZ and input A is ignored.
     *
     * Pinout (SOT-23-5 / DBV):
     *   1 = NC    (no connect)
     *   2 = A     (input)
     *   3 = GND
     *   4 = Y     (output)
     *   5 = VCC
     *
     * Function table (VCC powered):
     *   A    | Y
     *   -----|----
     *   Low  | Low
     *   High | High
     */
    class LVC1G17 final : public component::IPassive
    {
    public:
        /**
         * \enum Terminal
         * \brief Identifies the SOT-23-5 terminals of the LVC1G17.
         */
        enum class Terminal : std::uint8_t
        {
            NC,  ///< pin 1  No connect
            A,   ///< pin 2  Input
            GND, ///< pin 3  Ground
            Y,   ///< pin 4  Output
            VCC  ///< pin 5  Power supply (1.65V to 5.5V)
        };

        /**
         * \struct Descriptor
         * \brief Immutable configuration for LVC1G17 construction.
         */
        struct Descriptor
        {
            const char* name; ///< Component name (e.g. "U1")
        };

        /**
         * \brief Constructs an LVC1G17 from a descriptor.
         * \param descriptor Package configuration.
         */
        explicit LVC1G17(const Descriptor& descriptor);

        /**
         * \brief Returns the component name.
         */
        [[nodiscard]] virtual const char* name() const override;

        /**
         * \brief Returns the pin matching the given name.
         * \param name Pin name ("NC", "A", "GND", "Y", "VCC").
         */
        virtual signal::Pin& pin(const char* name) override;

        /**
         * \brief Returns the pin matching the given terminal identifier.
         * \param terminal Terminal identifier.
         */
        signal::Pin& pin(Terminal terminal);

        /**
         * \brief Returns all 5 pins in SOT-23-5 order.
         */
        [[nodiscard]] virtual std::span<signal::Pin* const> pins() const override;

    private:
        void onVCC(Voltage v);
        void compute();

        static constexpr float k_vccMin = 1.65f;
        static constexpr float k_vccMax = 5.5f;

        Descriptor m_descriptor;
        bool m_powered{ false };

        signal::Pin m_pinNC;
        signal::Pin m_pinA;
        signal::Pin m_pinGND;
        signal::Pin m_pinY;
        signal::Pin m_pinVCC;

        std::array<signal::Pin*, 5> m_pinPtrs;
    };
} // namespace nfx::silicon::buffer::sn74xxx
