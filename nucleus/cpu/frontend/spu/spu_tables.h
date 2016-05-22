/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/spu/spu_instruction.h"
#include "nucleus/cpu/frontend/spu/translator/spu_translator.h"

#include <string>

namespace cpu {
namespace frontend {
namespace spu {

enum EntryType {
    ENTRY_INVALID = 0,  // Entry does not match any PPU valid instruction or tables
    ENTRY_INSTRUCTION,  // Entry is an instruction
    ENTRY_TABLE,        // Entry is a table of entries
};

/**
 * PPU table entry
 */
struct Entry
{
    EntryType type;

    // Table data
    const Entry& (*caller)(Instruction);

    // Instruction data
    const char* name;
    void (Translator::*recompile)(Instruction);
};

// Instruction callers
const Entry& get_entry(Instruction code);

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
