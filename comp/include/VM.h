#pragma once
#include "instruction.h"
#include "Memory.h"
#include "CPU.h"
#include "Loader.h"

class VM {
public:
    void execute(const Executable& exe, bool debug = false);

private:
    Memory mem_;
    CPU    cpu_;
    Loader loader_;
};