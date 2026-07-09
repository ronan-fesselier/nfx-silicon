#pragma once

/**
 * \file InstructionSpec.h
 * \brief Generic instruction specification template shared by all CPU implementations.
 */

#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::internal::cpu
{
    /**
     * \struct InstructionSpec
     * \brief Generic instruction pattern with CPU-specific execute callback.
     *
     * \tparam CpuT    CPU type passed by reference to the execute callback.
     * \tparam OpcodeT Opcode scalar type (uint8_t for 8-bit CPUs, uint16_t for prefixed sets).
     */
    template <typename CpuT, typename OpcodeT>
    struct InstructionSpec
    {
        /**
         * \enum BusAccess
         * \brief S1 cycle bus access type, consumed by beginCycle() to select address and strobes.
         */
        enum class BusAccess : std::uint8_t
        {
            None, ///< no memory access this cycle
            Read, ///< memory read, address and /MRD set by beginCycle()
            Write ///< memory write, /MWR pulse issued inside execute()
        };

        OpcodeT mask;                      ///< bits to apply before comparing value
        OpcodeT value;                     ///< pattern value after masking
        const char* mnemonic;              ///< instruction mnemonic (for disassembly and debug)
        Cycles minCycles;                  ///< minimum machine cycles consumed
        Cycles maxCycles;                  ///< maximum machine cycles consumed (same as min for fixed-length)
        BusAccess busAccess;               ///< S1 bus access type for beginCycle()
        Cycles (*execute)(CpuT&, OpcodeT); ///< execute callback, returns actual cycles consumed
    };
} // namespace nfx::silicon::internal::cpu
