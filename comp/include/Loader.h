#pragma once
#include "instruction.h"
#include "Memory.h"

class Loader {
public:
    void load        (const Executable& exe, Memory& mem);
    void disassemble (const Executable& exe) const;
};