#include "VM.h"
#include <stdexcept>

void VM::execute(const Executable& exe, bool debug) {
    loader_.load(exe, mem_);
    if (debug) loader_.disassemble(exe);

    int ip = 0;
    while (cpu_.step(exe, ip, mem_)) { /* fetch-decode-execute */ }
}