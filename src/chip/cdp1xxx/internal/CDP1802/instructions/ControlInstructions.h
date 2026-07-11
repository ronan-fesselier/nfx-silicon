#pragma once

/**
 * \file ControlInstructions.h
 * \brief CDP1802 control instruction handlers (IDL, NOP, SEP, SEX, REQ, SEQ, SAV, MARK, RET, DIS).
 */

#include "chip/cdp1xxx/internal/CDP1802/ExecutionContext.h"

#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    [[nodiscard]] inline Cycles op_00(CDP1802& cpu, const std::uint8_t)
    {
        // IDL: wait for DMA or interrupt, M(R(0)) held on BUS (address driven by beginCycle)
        ExecutionContext::idling(cpu) = true;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_C4(CDP1802& /*cpu*/, const std::uint8_t)
    {
        // NOP: continue
        return Cycles{ 3 };
    }

    [[nodiscard]] inline Cycles op_Dn(CDP1802& cpu, const std::uint8_t opcode)
    {
        // SEP: N->P
        ExecutionContext::P(cpu) = static_cast<std::uint8_t>(opcode & 0x0Fu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_En(CDP1802& cpu, const std::uint8_t opcode)
    {
        // SEX: N->X
        ExecutionContext::X(cpu) = static_cast<std::uint8_t>(opcode & 0x0Fu);
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_7A(CDP1802& cpu, const std::uint8_t)
    {
        // REQ: 0->Q
        ExecutionContext::Q(cpu) = false;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_7B(CDP1802& cpu, const std::uint8_t)
    {
        // SEQ: 1->Q
        ExecutionContext::Q(cpu) = true;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_78(CDP1802& cpu, const std::uint8_t)
    {
        // SAV: T->M(R(X))
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        ExecutionContext::write(cpu, ExecutionContext::R(cpu, x), ExecutionContext::T(cpu));
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_79(CDP1802& cpu, const std::uint8_t)
    {
        // MARK: (X,P)->T, (X,P)->M(R(2)), P->X, R(2)--
        const auto xp = static_cast<std::uint8_t>(
            (static_cast<std::uint8_t>(ExecutionContext::X(cpu) << 4u)) | ExecutionContext::P(cpu));
        ExecutionContext::T(cpu) = xp;
        ExecutionContext::write(cpu, ExecutionContext::R(cpu, 2u), xp);
        ExecutionContext::X(cpu) = ExecutionContext::P(cpu);
        ExecutionContext::R(cpu, 2u)--;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_70(CDP1802& cpu, const std::uint8_t)
    {
        // RET: M(R(X))->(X,P), R(X)++, 1->IE
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        const auto val = ExecutionContext::read(cpu, ExecutionContext::R(cpu, x));
        ExecutionContext::X(cpu) = static_cast<std::uint8_t>((val >> 4u) & 0x0Fu);
        ExecutionContext::P(cpu) = static_cast<std::uint8_t>(val & 0x0Fu);
        ExecutionContext::R(cpu, x)++;
        ExecutionContext::IE(cpu) = true;
        return Cycles{ 2 };
    }

    [[nodiscard]] inline Cycles op_71(CDP1802& cpu, const std::uint8_t)
    {
        // DIS: M(R(X))->(X,P), R(X)++, 0->IE
        const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
        const auto val = ExecutionContext::read(cpu, ExecutionContext::R(cpu, x));
        ExecutionContext::X(cpu) = static_cast<std::uint8_t>((val >> 4u) & 0x0Fu);
        ExecutionContext::P(cpu) = static_cast<std::uint8_t>(val & 0x0Fu);
        ExecutionContext::R(cpu, x)++;
        ExecutionContext::IE(cpu) = false;
        return Cycles{ 2 };
    }
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
