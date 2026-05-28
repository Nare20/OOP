#include "Loader.h"
#include "utils.h"
#include <iostream>

void Loader::load(const Executable& exe, Memory& mem) {
    if (exe.code.empty())
        throw RuntimeError(0, "Empty executable");
    mem.reset();
}

void Loader::disassemble(const Executable& exe) const {
    std::cout << "=== Disassembly ===\n";
    for (int i = 0; i < (int)exe.code.size(); ++i) {
        for (auto& [name, addr] : exe.functions)
            if (addr == i) std::cout << "<" << name << ">:\n";
        std::cout << "  " << i << "\t" << exe.code[i].toString() << "\n";
    }
    std::cout << "===================\n\n";
}