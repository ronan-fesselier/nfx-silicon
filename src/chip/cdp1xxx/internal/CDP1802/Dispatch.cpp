#include "nfx/silicon/chip/cdp1xxx/CDP1802.h"
#include "instructions/LogicOperations.h"
#include "instructions/MemoryReference.h"
#include "instructions/RegisterOperations.h"
#include "internal/cpu/InstructionLookup.h"
#include "Spec.h"

#include "internal/runtime/Error.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <string_view>

namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
{
    namespace
    {
        // clang-format off
        // Opcode masks and values
        constexpr std::uint16_t MASK_NIBBLE_HIGH = 0x00F0;
        constexpr std::uint16_t MASK_FULL        = 0x00FF;

        // Register ops: match on high nibble
        constexpr std::uint16_t OP_LDN  = 0x0000; // 0n: Load via N
        constexpr std::uint16_t OP_INC  = 0x0010; // 1n: Increment Reg N
        constexpr std::uint16_t OP_DEC  = 0x0020; // 2n: Decrement Reg N
        constexpr std::uint16_t OP_LDA  = 0x0040; // 4n: Load advance
        constexpr std::uint16_t OP_STR  = 0x0050; // 5n: Store via N
        constexpr std::uint16_t OP_GLO  = 0x0080; // 8n: Get LOW Reg N
        constexpr std::uint16_t OP_GHI  = 0x0090; // 9n: Get HIGH Reg N
        constexpr std::uint16_t OP_PLO  = 0x00A0; // An: Put LOW Reg N
        constexpr std::uint16_t OP_PHI  = 0x00B0; // Bn: Put HIGH Reg N

        // 1-byte ops: full match
        constexpr std::uint16_t OP_IRX  = 0x0060; // 60: Increment Reg X
        constexpr std::uint16_t OP_LDXA = 0x0072; // 72: Load via X and advance
        constexpr std::uint16_t OP_STXD = 0x0073; // 73: Store via N and decrement
        constexpr std::uint16_t OP_LDX  = 0x00F0; // F0: Load via X
        constexpr std::uint16_t OP_LDI  = 0x00F8; // F8: Load immediate

        // Logic ops: full mask on 8-bit opcodes
        constexpr std::uint16_t OP_OR   = 0x00F1; // F1: OR via X
        constexpr std::uint16_t OP_AND  = 0x00F2; // F2: AND via X
        constexpr std::uint16_t OP_XOR  = 0x00F3; // F3: XOR via X
        constexpr std::uint16_t OP_SHR  = 0x00F6; // F6: shift right
        constexpr std::uint16_t OP_ORI  = 0x00F9; // F9: OR immediate
        constexpr std::uint16_t OP_ANI  = 0x00FA; // FA: AND immediate
        constexpr std::uint16_t OP_XRI  = 0x00FB; // FB: XOR immediate
        constexpr std::uint16_t OP_SHL  = 0x00FE; // FE: shift left

        // 7x ops (shared opcodes)
        constexpr std::uint16_t OP_SHRC = 0x0076; // 76: SHRC / RSHR
        constexpr std::uint16_t OP_SHLC = 0x007E; // 7E: SHLC / RSHL

        // Arithmetic ops: full mask
        constexpr std::uint16_t OP_ADD  = 0x00F4; // F4: ADD
        constexpr std::uint16_t OP_ADC  = 0x0074; // 74: ADC
        constexpr std::uint16_t OP_ADI  = 0x00FC; // FC: ADI
        constexpr std::uint16_t OP_ADCI = 0x007C; // 7C: ADCI
        constexpr std::uint16_t OP_SD   = 0x00F5; // F5: SD
        constexpr std::uint16_t OP_SDB  = 0x0075; // 75: SDB
        constexpr std::uint16_t OP_SDI  = 0x00FD; // FD: SDI
        constexpr std::uint16_t OP_SDBI = 0x007D; // 7D: SDBI
        constexpr std::uint16_t OP_SM   = 0x00F7; // F7: SM
        constexpr std::uint16_t OP_SMB  = 0x0077; // 77: SMB
        constexpr std::uint16_t OP_SMI  = 0x00FF; // FF: SMI
        constexpr std::uint16_t OP_SMBI = 0x007F; // 7F: SMBI

        // Short branch opcodes
        constexpr std::uint16_t OP_BR   = 0x0030; // 30: BR
        constexpr std::uint16_t OP_BQ   = 0x0031; // 31: BQ
        constexpr std::uint16_t OP_BZ   = 0x0032; // 32: BZ
        constexpr std::uint16_t OP_BDF  = 0x0033; // 33: BDF / BPZ / BGE
        constexpr std::uint16_t OP_BPZ  = 0x0033; // 33: BPZ (alias)
        constexpr std::uint16_t OP_BGE  = 0x0033; // 33: BGE (alias)
        constexpr std::uint16_t OP_B1   = 0x0034; // 34: B1
        constexpr std::uint16_t OP_B2   = 0x0035; // 35: B2
        constexpr std::uint16_t OP_B3   = 0x0036; // 36: B3
        constexpr std::uint16_t OP_B4   = 0x0037; // 37: B4
        constexpr std::uint16_t OP_NBR  = 0x0038; // 38: NBR / SKP
        constexpr std::uint16_t OP_BNQ  = 0x0039; // 39: BNQ
        constexpr std::uint16_t OP_BNZ  = 0x003A; // 3A: BNZ
        constexpr std::uint16_t OP_BNF  = 0x003B; // 3B: BNF / BM / BL
        constexpr std::uint16_t OP_BM   = 0x003B; // 3B: BM (alias)
        constexpr std::uint16_t OP_BL   = 0x003B; // 3B: BL (alias)
        constexpr std::uint16_t OP_BN1  = 0x003C; // 3C: BN1
        constexpr std::uint16_t OP_BN2  = 0x003D; // 3D: BN2
        constexpr std::uint16_t OP_BN3  = 0x003E; // 3E: BN3
        constexpr std::uint16_t OP_BN4  = 0x003F; // 3F: BN4

        // Long branch opcodes
        constexpr std::uint16_t OP_LBR  = 0x00C0; // C0: LBR
        constexpr std::uint16_t OP_LBQ  = 0x00C1; // C1: LBQ
        constexpr std::uint16_t OP_LBZ  = 0x00C2; // C2: LBZ
        constexpr std::uint16_t OP_LBDF = 0x00C3; // C3: LBDF
        constexpr std::uint16_t OP_NLBR = 0x00C8; // C8: NLBR / LSKP
        constexpr std::uint16_t OP_LBNQ = 0x00C9; // C9: LBNQ
        constexpr std::uint16_t OP_LBNZ = 0x00CA; // CA: LBNZ
        constexpr std::uint16_t OP_LBNF = 0x00CB; // CB: LBNF

        // Skip opcodes
        constexpr std::uint16_t OP_SKP  = 0x0038; // 38: SKP
        constexpr std::uint16_t OP_LSKP = 0x00C8; // C8: LSKP / NLBR
        constexpr std::uint16_t OP_LSNQ = 0x00C5; // C5: LSNQ
        constexpr std::uint16_t OP_LSNZ = 0x00C6; // C6: LSNZ
        constexpr std::uint16_t OP_LSNF = 0x00C7; // C7: LSNF
        constexpr std::uint16_t OP_LSZ  = 0x00CE; // CE: LSZ
        constexpr std::uint16_t OP_LSDF = 0x00CF; // CF: LSDF
        constexpr std::uint16_t OP_LSIE = 0x00CC; // CC: LSIE
        constexpr std::uint16_t OP_LSQ  = 0x00CD; // CD: LSQ

        // Control opcodes
        constexpr std::uint16_t OP_IDL  = 0x0000; // 00: IDL
        constexpr std::uint16_t OP_NOP  = 0x00C4; // C4: NOP
        constexpr std::uint16_t OP_SEP  = 0x00D0; // Dn: SEP
        constexpr std::uint16_t OP_SEX  = 0x00E0; // En: SEX
        constexpr std::uint16_t OP_REQ  = 0x007A; // 7A: REQ
        constexpr std::uint16_t OP_SEQ  = 0x007B; // 7B: SEQ
        constexpr std::uint16_t OP_SAV  = 0x0078; // 78: SAV
        constexpr std::uint16_t OP_MARK = 0x0079; // 79: MARK
        constexpr std::uint16_t OP_RET  = 0x0070; // 70: RET
        constexpr std::uint16_t OP_DIS  = 0x0071; // 71: DIS

        // I/O byte transfer opcodes
        constexpr std::uint16_t OP_OUT1 = 0x0061; // 61: OUT 1
        constexpr std::uint16_t OP_OUT2 = 0x0062; // 62: OUT 2
        constexpr std::uint16_t OP_OUT3 = 0x0063; // 63: OUT 3
        constexpr std::uint16_t OP_OUT4 = 0x0064; // 64: OUT 4
        constexpr std::uint16_t OP_OUT5 = 0x0065; // 65: OUT 5
        constexpr std::uint16_t OP_OUT6 = 0x0066; // 66: OUT 6
        constexpr std::uint16_t OP_OUT7 = 0x0067; // 67: OUT 7
        constexpr std::uint16_t OP_INP1 = 0x0069; // 69: INP 1
        constexpr std::uint16_t OP_INP2 = 0x006A; // 6A: INP 2
        constexpr std::uint16_t OP_INP3 = 0x006B; // 6B: INP 3
        constexpr std::uint16_t OP_INP4 = 0x006C; // 6C: INP 4
        constexpr std::uint16_t OP_INP5 = 0x006D; // 6D: INP 5
        constexpr std::uint16_t OP_INP6 = 0x006E; // 6E: INP 6
        constexpr std::uint16_t OP_INP7 = 0x006F; // 6F: INP 7
        // clang-format on

        // TODO: return actual cycle count once instruction handlers are implemented
        Cycles op_stub(CDP1802& /*cpu*/, std::uint8_t /*opcode*/)
        {
            return Cycles{ 2 };
        }

        Cycles op_unknown(CDP1802& /*cpu*/, const std::uint8_t opcode)
        {
            char msg[32]{};
            const int n = std::snprintf(msg, sizeof(msg), "unknown opcode 0x%02X", opcode);
            if (n > 0)
            {
                nfx::silicon::internal::runtime::error::log(
                    "CDP1802::dispatch",
                    nfx::silicon::internal::runtime::error::Level::Critical,
                    nfx::silicon::internal::runtime::error::Kind::Programming,
                    std::string_view{ msg, static_cast<std::size_t>(n) });
            }
            return Cycles{ 2 };
        }

        constexpr CDP1802InstructionSpec kUnknown{ .mask = MASK_FULL,
                                                   .value = OP_IDL,
                                                   .mnemonic = "UNK",
                                                   .minCycles = Cycles{ 2 },
                                                   .maxCycles = Cycles{ 2 },
                                                   .busAccess = CDP1802InstructionSpec::BusAccess::None,
                                                   .execute = &op_unknown };

        // Instruction table
        constexpr std::array<CDP1802InstructionSpec, 99> kInstructionTable{
            // clang-format off
            //                    { Mask            , Value  , Mnemonic, MinCycles  , MaxCycles  , BusAccess                               , Execute  }
            CDP1802InstructionSpec{ MASK_FULL       , OP_IDL , "IDL"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },

            // Memory reference
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_LDN , "LDN"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_0n },
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_LDA , "LDA"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_4n },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LDX , "LDX"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_F0 },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LDXA, "LDXA"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_72 },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LDI , "LDI"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_F8 },
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_STR , "STR"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_5n },
            CDP1802InstructionSpec{ MASK_FULL       , OP_STXD, "STXD"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_73 },

            // Register operations
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_INC , "INC"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_1n },
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_DEC , "DEC"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_2n },
            CDP1802InstructionSpec{ MASK_FULL       , OP_IRX , "IRX"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_60 },
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_GLO , "GLO"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_8n },
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_PLO , "PLO"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_An },
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_GHI , "GHI"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_9n },
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_PHI , "PHI"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_Bn },

            // Logic operations
            CDP1802InstructionSpec{ MASK_FULL       , OP_OR  , "OR"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_F1 },
            CDP1802InstructionSpec{ MASK_FULL       , OP_ORI , "ORI"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_F9 },
            CDP1802InstructionSpec{ MASK_FULL       , OP_XOR , "XOR"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_F3 },
            CDP1802InstructionSpec{ MASK_FULL       , OP_XRI , "XRI"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_FB },
            CDP1802InstructionSpec{ MASK_FULL       , OP_AND , "AND"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_F2 },
            CDP1802InstructionSpec{ MASK_FULL       , OP_ANI , "ANI"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_FA },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SHR , "SHR"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_F6 },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SHRC, "SHRC"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_76 },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SHRC, "RSHR"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_76 },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SHL , "SHL"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_FE },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SHLC, "SHLC"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_7E },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SHLC, "RSHL"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_7E },

            // Arithmetic operations
            CDP1802InstructionSpec{ MASK_FULL       , OP_ADD , "ADD"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_ADI , "ADI"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_ADC , "ADC"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_ADCI, "ADCI"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SD  , "SD"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SDI , "SDI"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SDB , "SDB"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SDBI, "SDBI"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SM  , "SM"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SMI , "SMI"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SMB , "SMB"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SMBI, "SMBI"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },

            // Short branch instructions
            CDP1802InstructionSpec{ MASK_FULL       , OP_BR  , "BR"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BQ  , "BQ"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BZ  , "BZ"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BDF , "BDF"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BPZ , "BPZ"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BGE , "BGE"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_B1  , "B1"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_B2  , "B2"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_B3  , "B3"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_B4  , "B4"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_NBR , "NBR"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BNQ , "BNQ"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BNZ , "BNZ"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BNF , "BNF"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BM  , "BM"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BL  , "BL"    , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BN1 , "BN1"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BN2 , "BN2"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BN3 , "BN3"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_BN4 , "BN4"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },

            // Long branch instructions
            CDP1802InstructionSpec{ MASK_FULL       , OP_LBR , "LBR"   , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_NLBR, "NLBR"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LBZ , "LBZ"   , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LBNZ, "LBNZ"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LBDF, "LBDF"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LBNF, "LBNF"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LBQ , "LBQ"   , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LBNQ, "LBNQ"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },

            // Skip instructions
            CDP1802InstructionSpec{ MASK_FULL       , OP_SKP , "SKP"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LSKP, "LSKP"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LSZ , "LSZ"   , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LSNZ, "LSNZ"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LSDF, "LSDF"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LSNF, "LSNF"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LSQ , "LSQ"   , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LSNQ, "LSNQ"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_LSIE, "LSIE"  , Cycles{ 3 }, Cycles{ 3 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },

            // Control instructions
            CDP1802InstructionSpec{ MASK_FULL       , OP_NOP , "NOP"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_stub },
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_SEP , "SEP"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_stub },
            CDP1802InstructionSpec{ MASK_NIBBLE_HIGH, OP_SEX , "SEX"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_REQ , "REQ"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SEQ , "SEQ"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::None , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_SAV , "SAV"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_MARK, "MARK"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_RET , "RET"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_DIS , "DIS"   , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },

            // Input - output byte transfer
            CDP1802InstructionSpec{ MASK_FULL       , OP_OUT1, "OUT1"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_OUT2, "OUT2"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_OUT3, "OUT3"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_OUT4, "OUT4"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_OUT5, "OUT5"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_OUT6, "OUT6"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_OUT7, "OUT7"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Read , &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_INP1, "INP1"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_INP2, "INP2"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_INP3, "INP3"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_INP4, "INP4"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_INP5, "INP5"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_INP6, "INP6"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_stub },
            CDP1802InstructionSpec{ MASK_FULL       , OP_INP7, "INP7"  , Cycles{ 2 }, Cycles{ 2 }, CDP1802InstructionSpec::BusAccess::Write, &op_stub }
            // clang-format on
        };
    } // namespace

    const CDP1802InstructionSpec& cdp1802Spec(const std::uint8_t opcode)
    {
        const auto* spec = internal::cpu::findInstruction<CDP1802, std::uint8_t>(
            opcode, kInstructionTable.data(), kInstructionTable.size());
        return (spec != nullptr) ? *spec : kUnknown;
    }

    nfx::silicon::Cycles cdp1802Dispatch(cdp1xxx::CDP1802& cpu, const std::uint8_t opcode)
    {
        const auto& spec = cdp1802Spec(opcode);
        return spec.execute(cpu, opcode);
    }
} // namespace nfx::silicon::chip::cdp1xxx::cdp1802internal
