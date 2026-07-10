#pragma once

/**
 * \file LogicOperations.h
 * \brief CDP1802 logic instruction handlers (OR, XOR, AND, SHR, SHL and their immediate/carry variants).
 */

#include "chip/cdp1xxx/internal/CDP1802/ExecutionContext.h"

#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    [[nodiscard]] inline Cycles op_F1(CDP1802& cpu, const std::uint8_t)
    {
        // OR: M(R(X)) OR D->D
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(
            ExecutionContext::read(cpu, ExecutionContext::R(cpu, x)) | ExecutionContext::D(cpu));
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_F9(CDP1802& cpu, const std::uint8_t)
    {
        // ORI: M(R(P)) OR D->D, R(P)+1->R(P)
        const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(
            ExecutionContext::read(cpu, ExecutionContext::R(cpu, p)) | ExecutionContext::D(cpu));
        ExecutionContext::R(cpu, p)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_F3(CDP1802& cpu, const std::uint8_t)
    {
        // XOR: M(R(X)) XOR D->D
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(
            ExecutionContext::read(cpu, ExecutionContext::R(cpu, x)) ^ ExecutionContext::D(cpu));
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_FB(CDP1802& cpu, const std::uint8_t)
    {
        // XRI: M(R(P)) XOR D->D, R(P)+1->R(P)
        const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(
            ExecutionContext::read(cpu, ExecutionContext::R(cpu, p)) ^ ExecutionContext::D(cpu));
        ExecutionContext::R(cpu, p)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_F2(CDP1802& cpu, const std::uint8_t)
    {
        // AND: M(R(X)) AND D->D
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(
            ExecutionContext::read(cpu, ExecutionContext::R(cpu, x)) & ExecutionContext::D(cpu));
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_FA(CDP1802& cpu, const std::uint8_t)
    {
        // ANI: M(R(P)) AND D->D, R(P)+1->R(P)
        const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(
            ExecutionContext::read(cpu, ExecutionContext::R(cpu, p)) & ExecutionContext::D(cpu));
        ExecutionContext::R(cpu, p)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_F6(CDP1802& cpu, const std::uint8_t)
    {
        // SHR: D>>1->D, D.0->DF, 0->D.7
        const auto d = ExecutionContext::D(cpu);
        ExecutionContext::DF(cpu) = (d & 0x01u) != 0u;
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(d >> 1u);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_76(CDP1802& cpu, const std::uint8_t)
    {
        // SHRC: D>>1->D, D.0->DF, DF->D.7
        const auto d = ExecutionContext::D(cpu);
        const bool oldDF = ExecutionContext::DF(cpu);
        ExecutionContext::DF(cpu) = (d & 0x01u) != 0u;
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>((d >> 1u) | (oldDF ? 0x80u : 0x00u));
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_FE(CDP1802& cpu, const std::uint8_t)
    {
        // SHL: D<<1->D, D.7->DF, 0->D.0
        const auto d = ExecutionContext::D(cpu);
        ExecutionContext::DF(cpu) = (d & 0x80u) != 0u;
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(d << 1u);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_7E(CDP1802& cpu, const std::uint8_t)
    {
        // SHLC: D<<1->D, D.7->DF, DF->D.0
        const auto d = ExecutionContext::D(cpu);
        const bool oldDF = ExecutionContext::DF(cpu);
        ExecutionContext::DF(cpu) = (d & 0x80u) != 0u;
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>((d << 1u) | (oldDF ? 0x01u : 0x00u));
        return Cycles{ 2 };
    }
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
