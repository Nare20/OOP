#include "VM.hpp"

VM::VM(int dataSize) {
    data.resize(dataSize, 0);
}

void VM::execute(const std::vector<Instruction>& code) {
    PC = 0;

    while (PC < (int)code.size()) {
        Instruction ins = code[PC++];

        switch (ins.op) {
            case OpCode::LOADI:
                R[ins.a] = ins.b;
                break;

            case OpCode::LOAD:
                R[ins.a] = data[ins.b];
                break;

            case OpCode::STORE:
                data[ins.b] = R[ins.a];
                break;

            case OpCode::MOVE:
                R[ins.a] = R[ins.b];
                break;

            case OpCode::ADD:
                R[ins.a] = R[ins.a] + R[ins.b];
                break;

            case OpCode::SUB:
                R[ins.a] = R[ins.a] - R[ins.b];
                break;

            case OpCode::MUL:
                R[ins.a] = R[ins.a] * R[ins.b];
                break;

            case OpCode::DIV:
                R[ins.a] = R[ins.a] / R[ins.b];
                break;

            case OpCode::HALT:
                return;
        }
    }
}