#pragma once

/**
 * \file RegisterOperations.h
 * \brief CDP1802 register operation handlers (INC, DEC, IRX, GLO, GHI, PLO, PHI).
 */

#include "chip/cdp1xxx/internal/CDP1802/ExecutionContext.h"

#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    [[nodiscard]] inline Cycles op_1n(CDP1802& cpu, const std::uint8_t opcode)
    {
        // INC: R(N)+1->R(N)
        const auto n = static_cast<std::size_t>(opcode & 0x0Fu);
        ExecutionContext::R(cpu, n) = static_cast<std::uint16_t>(ExecutionContext::R(cpu, n) + 1u);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_2n(CDP1802& cpu, const std::uint8_t opcode)
    {
        // DEC: R(N)-1->R(N)
        const auto n = static_cast<std::size_t>(opcode & 0x0Fu);
        ExecutionContext::R(cpu, n) = static_cast<std::uint16_t>(ExecutionContext::R(cpu, n) - 1u);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_60(CDP1802& cpu, const std::uint8_t)
    {
        // IRX: R(X)+1->R(X)
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        ExecutionContext::R(cpu, x) = static_cast<std::uint16_t>(ExecutionContext::R(cpu, x) + 1u);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_8n(CDP1802& cpu, const std::uint8_t opcode)
    {
        // GLO: R(N).lo->D
        const auto n = static_cast<std::size_t>(opcode & 0x0Fu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(ExecutionContext::R(cpu, n) & 0xFFu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_9n(CDP1802& cpu, const std::uint8_t opcode)
    {
        // GHI: R(N).hi->D
        const auto n = static_cast<std::size_t>(opcode & 0x0Fu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>((ExecutionContext::R(cpu, n) >> 8u) & 0xFFu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_An(CDP1802& cpu, const std::uint8_t opcode)
    {
        // PLO: D->R(N).lo
        const auto n = static_cast<std::size_t>(opcode & 0x0Fu);
        ExecutionContext::R(cpu, n) =
            static_cast<std::uint16_t>((ExecutionContext::R(cpu, n) & 0xFF00u) | ExecutionContext::D(cpu));
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_Bn(CDP1802& cpu, const std::uint8_t opcode)
    {
        // PHI: D->R(N).hi
        const auto n = static_cast<std::size_t>(opcode & 0x0Fu);
        ExecutionContext::R(cpu, n) = static_cast<std::uint16_t>(
            (static_cast<std::uint16_t>(ExecutionContext::D(cpu)) << 8u) | (ExecutionContext::R(cpu, n) & 0x00FFu));
        return Cycles{ 2 };
    }
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
