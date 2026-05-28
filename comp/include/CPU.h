#pragma once
#include "instruction.h"
#include "Memory.h"
#include <stdexcept>

class CPU {
public:
    // Returns false when HALT is reached
    bool step(const Executable& exe, int& ip, Memory& mem);

private:
    Value applyBinaryOp(OpCode op, const Value& a, const Value& b);
};