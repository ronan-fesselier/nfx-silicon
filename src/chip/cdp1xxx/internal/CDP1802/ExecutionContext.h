#pragma once

/**
 * \file ExecutionContext.h
 * \brief Accessor shim granting instruction handlers access to CDP1802 internals.
 */

#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstddef>
#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    struct ExecutionContext
    {
        static std::uint8_t& D(CDP1802& cpu) { return cpu.m_registers.D; }
        static bool& DF(CDP1802& cpu) { return cpu.m_registers.DF; }
        static std::uint8_t& B(CDP1802& cpu) { return cpu.m_registers.B; }
        static std::uint16_t& R(CDP1802& cpu, std::size_t i) { return cpu.m_registers.R[i]; }
        static std::uint8_t& P(CDP1802& cpu) { return cpu.m_registers.P; }
        static std::uint8_t& X(CDP1802& cpu) { return cpu.m_registers.X; }
        static std::uint8_t& N(CDP1802& cpu) { return cpu.m_registers.N; }
        static std::uint8_t& I(CDP1802& cpu) { return cpu.m_registers.I; }
        static std::uint8_t& T(CDP1802& cpu) { return cpu.m_registers.T; }
        static bool& IE(CDP1802& cpu) { return cpu.m_registers.IE; }
        static bool& Q(CDP1802& cpu) { return cpu.m_registers.Q; }
        static bool& idling(CDP1802& cpu) { return cpu.m_exec.idling; }
        static std::uint8_t& phase(CDP1802& cpu) { return cpu.m_exec.executePhase; }

        static bool EF(const CDP1802& cpu, std::size_t i) { return cpu.m_io.efSampled[i]; }

        static std::uint8_t read(CDP1802& cpu, const std::uint16_t addr)
        {
            cpu.driveAddress(addr, true);
            cpu.driveAddress(addr, false);
            cpu.m_pinNMRD.drive(Level::Low);
            cpu.m_pinTPB.drive(Level::High);
            const std::uint8_t data = cpu.readBus();
            cpu.m_pinTPB.drive(Level::Low);
            cpu.m_pinNMRD.drive(Level::High);
            return data;
        }

        static std::uint8_t readBus(CDP1802& cpu) { return cpu.readBus(); }

        static void write(CDP1802& cpu, const std::uint16_t addr, const std::uint8_t data)
        {
            cpu.driveAddress(addr, true);
            cpu.driveAddress(addr, false);
            cpu.driveBus(data);
            cpu.m_pinNMWR.drive(Level::Low);
            cpu.m_exec.didWrite = true;
        }
    };
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
