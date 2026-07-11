#pragma once

/**
 * \file SkipInstructions.h
 * \brief CDP1802 long skip instruction handlers (LSZ, LSNZ, LSDF, LSNF, LSQ, LSNQ, LSIE).
 */

#include "chip/cdp1xxx/internal/CDP1802/ExecutionContext.h"

#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    // Long skips: R(P)++ per phase when taken (2 real cycles, +1 each), else no-op both phases.
    namespace
    {
        inline void longSkip(CDP1802& cpu, const bool take)
        {
            if (!take)
            {
                return;
            }
            const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
            ExecutionContext::R(cpu, p)++;
        }
    } // namespace

    [[nodiscard]] inline Cycles op_C5(CDP1802& cpu, const std::uint8_t)
    {
        // LSNQ: IF Q=0, R(P)+=2
        longSkip(cpu, !ExecutionContext::Q(cpu));
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_C6(CDP1802& cpu, const std::uint8_t)
    {
        // LSNZ: IF D!=0, R(P)+=2
        longSkip(cpu, ExecutionContext::D(cpu) != 0u);
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_C7(CDP1802& cpu, const std::uint8_t)
    {
        // LSNF: IF DF=0, R(P)+=2
        longSkip(cpu, !ExecutionContext::DF(cpu));
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_CC(CDP1802& cpu, const std::uint8_t)
    {
        // LSIE: IF IE=1, R(P)+=2
        longSkip(cpu, ExecutionContext::IE(cpu));
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_CD(CDP1802& cpu, const std::uint8_t)
    {
        // LSQ: IF Q=1, R(P)+=2
        longSkip(cpu, ExecutionContext::Q(cpu));
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_CE(CDP1802& cpu, const std::uint8_t)
    {
        // LSZ: IF D=0, R(P)+=2
        longSkip(cpu, ExecutionContext::D(cpu) == 0u);
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_CF(CDP1802& cpu, const std::uint8_t)
    {
        // LSDF: IF DF=1, R(P)+=2
        longSkip(cpu, ExecutionContext::DF(cpu));
        return Cycles{ 3 };
    }
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
