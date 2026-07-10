#pragma once

/**
 * \file MemoryReference.h
 * \brief CDP1802 memory reference instruction handlers (LDN, LDA, LDX, LDXA, LDI, STR, STXD).
 */

#include "chip/cdp1xxx/internal/CDP1802/ExecutionContext.h"

#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    [[nodiscard]] inline Cycles op_0n(CDP1802& cpu, const std::uint8_t opcode)
    {
        // LDN: M(R(N))->D for N!=0
        const auto n = static_cast<std::size_t>(opcode & 0x0Fu);
        if (n != 0)
        {
            ExecutionContext::D(cpu) = ExecutionContext::read(cpu, ExecutionContext::R(cpu, n));
        }
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_4n(CDP1802& cpu, const std::uint8_t opcode)
    {
        // LDA: M(R(N))->D, R(N)+1->R(N)
        const auto n = static_cast<std::size_t>(opcode & 0x0Fu);
        ExecutionContext::D(cpu) = ExecutionContext::read(cpu, ExecutionContext::R(cpu, n));
        ExecutionContext::R(cpu, n)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_F0(CDP1802& cpu, const std::uint8_t)
    {
        // LDX: M(R(X))->D
        ExecutionContext::D(cpu) = ExecutionContext::read(cpu, ExecutionContext::R(cpu, ExecutionContext::X(cpu)));
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_72(CDP1802& cpu, const std::uint8_t)
    {
        // LDXA: M(R(X))->D, R(X)+1->R(X)
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        ExecutionContext::D(cpu) = ExecutionContext::read(cpu, ExecutionContext::R(cpu, x));
        ExecutionContext::R(cpu, x)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_F8(CDP1802& cpu, const std::uint8_t)
    {
        // LDI: M(R(P))->D, R(P)+1->R(P)
        // beginCycle() already drove R(P) on MA and nMRD Low -- sample directly
        ExecutionContext::D(cpu) = ExecutionContext::readBus(cpu);
        ExecutionContext::R(cpu, ExecutionContext::P(cpu))++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_5n(CDP1802& cpu, const std::uint8_t opcode)
    {
        // STR: D->M(R(N))
        const auto n = static_cast<std::size_t>(opcode & 0x0Fu);
        ExecutionContext::write(cpu, ExecutionContext::R(cpu, n), ExecutionContext::D(cpu));
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_73(CDP1802& cpu, const std::uint8_t)
    {
        // STXD: D->M(R(X)), R(X)-1->R(X)
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        ExecutionContext::write(cpu, ExecutionContext::R(cpu, x), ExecutionContext::D(cpu));
        ExecutionContext::R(cpu, x)--;
        return Cycles{ 2 };
    }
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
