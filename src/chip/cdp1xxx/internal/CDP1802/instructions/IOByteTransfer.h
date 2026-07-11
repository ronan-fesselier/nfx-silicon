#pragma once

/**
 * \file IOByteTransfer.h
 * \brief CDP1802 I/O byte transfer instruction handlers (OUT 1..7, INP 1..7).
 */

#include "chip/cdp1xxx/internal/CDP1802/ExecutionContext.h"

#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    // OUT n: M(R(X))->BUS, R(X)++  (N lines driven by beginCycle via m_registers.N)
    // INP n: BUS->M(R(X)), BUS->D  (R(X) not incremented)
    namespace
    {
        [[nodiscard]] inline Cycles outN(CDP1802& cpu)
        {
            const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
            ExecutionContext::read(cpu, ExecutionContext::R(cpu, x));
            ExecutionContext::R(cpu, x)++;
            return Cycles{ 2 };
        }

        [[nodiscard]] inline Cycles inpN(CDP1802& cpu)
        {
            const auto x = static_cast<std::size_t>(ExecutionContext::X(cpu));
            const auto data = ExecutionContext::readBus(cpu);
            ExecutionContext::write(cpu, ExecutionContext::R(cpu, x), data);
            ExecutionContext::D(cpu) = data;
            return Cycles{ 2 };
        }
    } // namespace

    [[nodiscard]] inline Cycles op_61(CDP1802& cpu, const std::uint8_t)
    {
        return outN(cpu);
    } // OUT 1
    [[nodiscard]] inline Cycles op_62(CDP1802& cpu, const std::uint8_t)
    {
        return outN(cpu);
    } // OUT 2
    [[nodiscard]] inline Cycles op_63(CDP1802& cpu, const std::uint8_t)
    {
        return outN(cpu);
    } // OUT 3
    [[nodiscard]] inline Cycles op_64(CDP1802& cpu, const std::uint8_t)
    {
        return outN(cpu);
    } // OUT 4
    [[nodiscard]] inline Cycles op_65(CDP1802& cpu, const std::uint8_t)
    {
        return outN(cpu);
    } // OUT 5
    [[nodiscard]] inline Cycles op_66(CDP1802& cpu, const std::uint8_t)
    {
        return outN(cpu);
    } // OUT 6
    [[nodiscard]] inline Cycles op_67(CDP1802& cpu, const std::uint8_t)
    {
        return outN(cpu);
    } // OUT 7

    [[nodiscard]] inline Cycles op_69(CDP1802& cpu, const std::uint8_t)
    {
        return inpN(cpu);
    } // INP 1
    [[nodiscard]] inline Cycles op_6A(CDP1802& cpu, const std::uint8_t)
    {
        return inpN(cpu);
    } // INP 2
    [[nodiscard]] inline Cycles op_6B(CDP1802& cpu, const std::uint8_t)
    {
        return inpN(cpu);
    } // INP 3
    [[nodiscard]] inline Cycles op_6C(CDP1802& cpu, const std::uint8_t)
    {
        return inpN(cpu);
    } // INP 4
    [[nodiscard]] inline Cycles op_6D(CDP1802& cpu, const std::uint8_t)
    {
        return inpN(cpu);
    } // INP 5
    [[nodiscard]] inline Cycles op_6E(CDP1802& cpu, const std::uint8_t)
    {
        return inpN(cpu);
    } // INP 6
    [[nodiscard]] inline Cycles op_6F(CDP1802& cpu, const std::uint8_t)
    {
        return inpN(cpu);
    } // INP 7
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
