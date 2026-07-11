#pragma once

/**
 * \file BranchInstructions.h
 * \brief CDP1802 branch instruction handlers (short: 3x, long: Cx).
 */

#include "chip/cdp1xxx/internal/CDP1802/ExecutionContext.h"

#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    // Short branch helpers
    // Always fetch M(R(P)) into scratch, then branch (R(P).0 = byte) or skip (R(P)++)
    namespace
    {
        [[nodiscard]] inline std::uint8_t shortBranchFetch(CDP1802& cpu)
        {
            return ExecutionContext::readBus(cpu);
        }

        inline void shortBranchTake(CDP1802& cpu, const std::uint8_t addr)
        {
            const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
            ExecutionContext::R(cpu, p) =
                static_cast<std::uint16_t>((ExecutionContext::R(cpu, p) & 0xFF00u) | static_cast<std::uint16_t>(addr));
        }

        inline void shortBranchSkip(CDP1802& cpu)
        {
            const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
            ExecutionContext::R(cpu, p)++;
        }

        inline void longBranch(CDP1802& cpu, const bool take)
        {
            const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
            if (ExecutionContext::phase(cpu) == 0u)
            {
                if (take)
                {
                    ExecutionContext::B(cpu) = ExecutionContext::read(cpu, ExecutionContext::R(cpu, p));
                }
                else
                {
                    ExecutionContext::R(cpu, p)++;
                }
            }
            else
            {
                if (take)
                {
                    const std::uint8_t lo =
                        ExecutionContext::read(cpu, static_cast<std::uint16_t>(ExecutionContext::R(cpu, p) + 1u));
                    ExecutionContext::R(cpu, p) = static_cast<std::uint16_t>(
                        (static_cast<std::uint16_t>(ExecutionContext::B(cpu)) << 8u) | static_cast<std::uint16_t>(lo));
                }
                else
                {
                    ExecutionContext::R(cpu, p)++;
                }
            }
        }
    } // namespace

    // Short branch instructions (3x)
    [[nodiscard]] inline Cycles op_30(CDP1802& cpu, const std::uint8_t)
    {
        // BR: M(R(P))->R(P).0
        const std::uint8_t addr = shortBranchFetch(cpu);
        shortBranchTake(cpu, addr);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_31(CDP1802& cpu, const std::uint8_t)
    {
        // BQ: IF Q=1, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (ExecutionContext::Q(cpu))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_32(CDP1802& cpu, const std::uint8_t)
    {
        // BZ: IF D=0, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (ExecutionContext::D(cpu) == 0u)
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_33(CDP1802& cpu, const std::uint8_t)
    {
        // BDF: IF DF=1, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (ExecutionContext::DF(cpu))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_34(CDP1802& cpu, const std::uint8_t)
    {
        // B1: IF EF1=1, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (ExecutionContext::EF(cpu, 0u))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_35(CDP1802& cpu, const std::uint8_t)
    {
        // B2: IF EF2=1, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (ExecutionContext::EF(cpu, 1u))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_36(CDP1802& cpu, const std::uint8_t)
    {
        // B3: IF EF3=1, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (ExecutionContext::EF(cpu, 2u))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_37(CDP1802& cpu, const std::uint8_t)
    {
        // B4: IF EF4=1, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (ExecutionContext::EF(cpu, 3u))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_38(CDP1802& cpu, const std::uint8_t)
    {
        // NBR/SKP: R(P)++ (always skip the branch byte)
        shortBranchSkip(cpu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_39(CDP1802& cpu, const std::uint8_t)
    {
        // BNQ: IF Q=0, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (!ExecutionContext::Q(cpu))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_3A(CDP1802& cpu, const std::uint8_t)
    {
        // BNZ: IF D!=0, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (ExecutionContext::D(cpu) != 0u)
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_3B(CDP1802& cpu, const std::uint8_t)
    {
        // BNF: IF DF=0, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (!ExecutionContext::DF(cpu))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_3C(CDP1802& cpu, const std::uint8_t)
    {
        // BN1: IF EF1=0, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (!ExecutionContext::EF(cpu, 0u))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_3D(CDP1802& cpu, const std::uint8_t)
    {
        // BN2: IF EF2=0, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (!ExecutionContext::EF(cpu, 1u))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_3E(CDP1802& cpu, const std::uint8_t)
    {
        // BN3: IF EF3=0, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (!ExecutionContext::EF(cpu, 2u))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_3F(CDP1802& cpu, const std::uint8_t)
    {
        // BN4: IF EF4=0, M(R(P))->R(P).0, ELSE R(P)++
        const std::uint8_t addr = shortBranchFetch(cpu);
        if (!ExecutionContext::EF(cpu, 3u))
        {
            shortBranchTake(cpu, addr);
        }
        else
        {
            shortBranchSkip(cpu);
        }
        return Cycles{ 2 };
    }

    // Long branch instructions (Cx)
    [[nodiscard]] inline Cycles op_C0(CDP1802& cpu, const std::uint8_t)
    {
        // LBR: B<-M(R(P)), R(P)<-(B<<8)|M(R(P)+1)
        longBranch(cpu, true);
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_C1(CDP1802& cpu, const std::uint8_t)
    {
        // LBQ: IF Q=1, long branch, ELSE R(P)+=2
        longBranch(cpu, ExecutionContext::Q(cpu));
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_C2(CDP1802& cpu, const std::uint8_t)
    {
        // LBZ: IF D=0, long branch, ELSE R(P)+=2
        longBranch(cpu, ExecutionContext::D(cpu) == 0u);
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_C3(CDP1802& cpu, const std::uint8_t)
    {
        // LBDF: IF DF=1, long branch, ELSE R(P)+=2
        longBranch(cpu, ExecutionContext::DF(cpu));
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_C8(CDP1802& cpu, const std::uint8_t)
    {
        // NLBR/LSKP: R(P)+=2 (always skip 2 bytes)
        longBranch(cpu, false);
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_C9(CDP1802& cpu, const std::uint8_t)
    {
        // LBNQ: IF Q=0, long branch, ELSE R(P)+=2
        longBranch(cpu, !ExecutionContext::Q(cpu));
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_CA(CDP1802& cpu, const std::uint8_t)
    {
        // LBNZ: IF D!=0, long branch, ELSE R(P)+=2
        longBranch(cpu, ExecutionContext::D(cpu) != 0u);
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_CB(CDP1802& cpu, const std::uint8_t)
    {
        // LBNF: IF DF=0, long branch, ELSE R(P)+=2
        longBranch(cpu, !ExecutionContext::DF(cpu));
        return Cycles{ 3 };
    }
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
