#pragma once

/**
 * \file ArithmeticOperations.h
 * \brief CDP1802 arithmetic instruction handlers (ADD, ADC, SD, SDB, SM, SMB and their immediate variants).
 */

#include "chip/cdp1xxx/internal/CDP1802/ExecutionContext.h"

#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    [[nodiscard]] inline Cycles op_F4(CDP1802& cpu, const std::uint8_t)
    {
        // ADD: M(R(X))+D->DF,D
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        const std::uint16_t sum = static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, x))) +
                                  static_cast<std::uint16_t>(ExecutionContext::D(cpu));
        ExecutionContext::DF(cpu) = (sum > 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(sum & 0xFFu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_FC(CDP1802& cpu, const std::uint8_t)
    {
        // ADI: M(R(P))+D->DF,D, R(P)+1->R(P)
        const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
        const std::uint16_t sum = static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, p))) +
                                  static_cast<std::uint16_t>(ExecutionContext::D(cpu));
        ExecutionContext::DF(cpu) = (sum > 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(sum & 0xFFu);
        ExecutionContext::R(cpu, p)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_74(CDP1802& cpu, const std::uint8_t)
    {
        // ADC: M(R(X))+D+DF->DF,D
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        const std::uint16_t sum = static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, x))) +
                                  static_cast<std::uint16_t>(ExecutionContext::D(cpu)) +
                                  (ExecutionContext::DF(cpu) ? 1u : 0u);
        ExecutionContext::DF(cpu) = (sum > 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(sum & 0xFFu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_7C(CDP1802& cpu, const std::uint8_t)
    {
        // ADCI: M(R(P))+D+DF->DF,D, R(P)+1->R(P)
        const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
        const std::uint16_t sum = static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, p))) +
                                  static_cast<std::uint16_t>(ExecutionContext::D(cpu)) +
                                  (ExecutionContext::DF(cpu) ? 1u : 0u);
        ExecutionContext::DF(cpu) = (sum > 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(sum & 0xFFu);
        ExecutionContext::R(cpu, p)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_F5(CDP1802& cpu, const std::uint8_t)
    {
        // SD: M(R(X))-D->DF,D
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        const std::uint16_t diff =
            static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, x))) -
            static_cast<std::uint16_t>(ExecutionContext::D(cpu));
        ExecutionContext::DF(cpu) = (diff <= 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(diff & 0xFFu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_FD(CDP1802& cpu, const std::uint8_t)
    {
        // SDI: M(R(P))-D->DF,D, R(P)+1->R(P)
        const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
        const std::uint16_t diff =
            static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, p))) -
            static_cast<std::uint16_t>(ExecutionContext::D(cpu));
        ExecutionContext::DF(cpu) = (diff <= 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(diff & 0xFFu);
        ExecutionContext::R(cpu, p)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_75(CDP1802& cpu, const std::uint8_t)
    {
        // SDB: M(R(X))-D-(NOT DF)->DF,D
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        const std::uint16_t diff =
            static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, x))) -
            static_cast<std::uint16_t>(ExecutionContext::D(cpu)) - (ExecutionContext::DF(cpu) ? 0u : 1u);
        ExecutionContext::DF(cpu) = (diff <= 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(diff & 0xFFu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_7D(CDP1802& cpu, const std::uint8_t)
    {
        // SDBI: M(R(P))-D-(NOT DF)->DF,D, R(P)+1->R(P)
        const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
        const std::uint16_t diff =
            static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, p))) -
            static_cast<std::uint16_t>(ExecutionContext::D(cpu)) - (ExecutionContext::DF(cpu) ? 0u : 1u);
        ExecutionContext::DF(cpu) = (diff <= 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(diff & 0xFFu);
        ExecutionContext::R(cpu, p)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_F7(CDP1802& cpu, const std::uint8_t)
    {
        // SM: D-M(R(X))->DF,D
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        const std::uint16_t diff = static_cast<std::uint16_t>(ExecutionContext::D(cpu)) -
                                   static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, x)));
        ExecutionContext::DF(cpu) = (diff <= 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(diff & 0xFFu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_FF(CDP1802& cpu, const std::uint8_t)
    {
        // SMI: D-M(R(P))->DF,D, R(P)+1->R(P)
        const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
        const std::uint16_t diff = static_cast<std::uint16_t>(ExecutionContext::D(cpu)) -
                                   static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, p)));
        ExecutionContext::DF(cpu) = (diff <= 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(diff & 0xFFu);
        ExecutionContext::R(cpu, p)++;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_77(CDP1802& cpu, const std::uint8_t)
    {
        // SMB: D-M(R(X))-(NOT DF)->DF,D
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        const std::uint16_t diff =
            static_cast<std::uint16_t>(ExecutionContext::D(cpu)) -
            static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, x))) -
            (ExecutionContext::DF(cpu) ? 0u : 1u);
        ExecutionContext::DF(cpu) = (diff <= 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(diff & 0xFFu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_7F(CDP1802& cpu, const std::uint8_t)
    {
        // SMBI: D-M(R(P))-(NOT DF)->DF,D, R(P)+1->R(P)
        const auto p = static_cast<std::size_t>(ExecutionContext::P(cpu));
        const std::uint16_t diff =
            static_cast<std::uint16_t>(ExecutionContext::D(cpu)) -
            static_cast<std::uint16_t>(ExecutionContext::read(cpu, ExecutionContext::R(cpu, p))) -
            (ExecutionContext::DF(cpu) ? 0u : 1u);
        ExecutionContext::DF(cpu) = (diff <= 0xFFu);
        ExecutionContext::D(cpu) = static_cast<std::uint8_t>(diff & 0xFFu);
        ExecutionContext::R(cpu, p)++;
        return Cycles{ 2 };
    }
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
