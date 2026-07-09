#pragma once

#include "internal/cpu/InstructionSpec.h"
#include "internal/cpu/InstructionLookup.h"
#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "nfx/silicon/Types.h"

#include <cstdint>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    using CDP1802InstructionSpec = internal::cpu::InstructionSpec<cdp1xxx::CDP1802, std::uint8_t>;

    /**
     * \brief Returns the instruction spec for the given CDP1802 opcode byte.
     * \param opcode Opcode byte (I<<4 | N).
     * \return Matching instruction spec, or unknown fallback spec.
     */
    const CDP1802InstructionSpec& cdp1802Spec(std::uint8_t opcode);

    /**
     * \brief Dispatches and executes the given CDP1802 opcode.
     * \param cpu    CPU instance for this S1 cycle.
     * \param opcode Opcode byte (I<<4 | N).
     * \return Actual machine cycles consumed.
     */
    nfx::silicon::Cycles cdp1802Dispatch(cdp1xxx::CDP1802& cpu, std::uint8_t opcode);
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
