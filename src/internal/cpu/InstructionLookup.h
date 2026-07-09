#pragma once

/**
 * \file InstructionLookup.h
 * \brief Template helpers for matching and finding instruction specs by opcode.
 */

#include "InstructionSpec.h"

#include <cstddef>

namespace nfx::silicon::internal::cpu
{
    /**
     * \brief Returns true when opcode matches an instruction spec mask/value pattern.
     * \tparam CpuT    CPU type.
     * \tparam OpcodeT Opcode scalar type.
     * \param opcode Opcode value to test.
     * \param spec   Instruction specification to match against.
     * \return True when (opcode & spec.mask) == spec.value.
     */
    template <typename CpuT, typename OpcodeT>
    [[nodiscard]] constexpr bool matchesInstruction(const OpcodeT opcode, const InstructionSpec<CpuT, OpcodeT>& spec)
    {
        return static_cast<OpcodeT>(opcode & spec.mask) == static_cast<OpcodeT>(spec.value);
    }

    /**
     * \brief Returns the first matching instruction spec using first-match-wins precedence.
     * \tparam CpuT    CPU type.
     * \tparam OpcodeT Opcode scalar type.
     * \param opcode Opcode value to decode.
     * \param table  Pointer to the first entry in the instruction table.
     * \param count  Number of entries in the table.
     * \return Pointer to the matched spec, or nullptr if no entry matches.
     */
    template <typename CpuT, typename OpcodeT>
    [[nodiscard]] constexpr const InstructionSpec<CpuT, OpcodeT>* findInstruction(
        const OpcodeT opcode, const InstructionSpec<CpuT, OpcodeT>* table, const std::size_t count)
    {
        if (table == nullptr || count == 0)
        {
            return nullptr;
        }

        for (std::size_t i = 0; i < count; ++i)
        {
            if (matchesInstruction(opcode, table[i]))
            {
                return &table[i];
            }
        }

        return nullptr;
    }
} // namespace nfx::silicon::internal::cpu
