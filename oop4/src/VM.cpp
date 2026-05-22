#include "VM.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>

VM::VM(){
    for(int i = 0; i < REGISTER_COUNT; ++i) regs[i] = 0;
    for(int i = 0; i < MEMORY_WORDS; ++i) memory[i] = 0;
}

bool VM::load(const std::vector<uint8_t> &bytecode){
    code = bytecode;
    ip = 0;
    SP = MEMORY_WORDS;
    FP = MEMORY_WORDS;
    for(int i = 0; i < REGISTER_COUNT; ++i) regs[i] = 0;
    // Push a sentinel return address for the top-level entry frame.
    push(-1);
    return true;
}

void VM::push(int64_t v){
    if(SP <= 0) throw std::runtime_error("stack overflow");
    memory[--SP] = v;
}

int64_t VM::pop(){
    if(SP >= MEMORY_WORDS) throw std::runtime_error("stack underflow");
    return memory[SP++];
}

uint8_t VM::readU8(){ if(ip >= code.size()) throw std::runtime_error("code read out of range"); return code[ip++]; }
uint32_t VM::readU32(){ if(ip + 4 > code.size()) throw std::runtime_error("code read out of range"); uint32_t value; std::memcpy(&value, &code[ip], 4); ip += 4; return value; }
int64_t VM::readI64(){ if(ip + 8 > code.size()) throw std::runtime_error("code read out of range"); int64_t value; std::memcpy(&value, &code[ip], 8); ip += 8; return value; }

int VM::run(){
    while(ip < code.size()){
        Op op = static_cast<Op>(readU8());
        switch(op){
            case OP_HALT:
                return 0;
            case OP_PUSH_CONST: {
                int64_t value = readI64();
                push(value);
                break;
            }
            case OP_LOAD_VAR: {
                int32_t offset = static_cast<int32_t>(readU32());
                int idx = FP + offset / 8;
                push(memory[idx]);
                break;
            }
            case OP_STORE_VAR: {
                int32_t offset = static_cast<int32_t>(readU32());
                int64_t value = pop();
                int idx = FP + offset / 8;
                memory[idx] = value;
                break;
            }
            case OP_ADD: {
                regs[1] = pop();
                regs[0] = pop();
                regs[0] += regs[1];
                push(regs[0]);
                break;
            }
            case OP_SUB: {
                regs[1] = pop();
                regs[0] = pop();
                regs[0] -= regs[1];
                push(regs[0]);
                break;
            }
            case OP_MUL: {
                regs[1] = pop();
                regs[0] = pop();
                regs[0] *= regs[1];
                push(regs[0]);
                break;
            }
            case OP_DIV: {
                regs[1] = pop();
                regs[0] = pop();
                regs[0] /= regs[1];
                push(regs[0]);
                break;
            }
            case OP_CMP_EQ: {
                regs[1] = pop();
                regs[0] = pop();
                push(regs[0] == regs[1] ? 1 : 0);
                break;
            }
            case OP_CMP_NE: {
                regs[1] = pop();
                regs[0] = pop();
                push(regs[0] != regs[1] ? 1 : 0);
                break;
            }
            case OP_CMP_LT: {
                regs[1] = pop();
                regs[0] = pop();
                push(regs[0] < regs[1] ? 1 : 0);
                break;
            }
            case OP_CMP_LE: {
                regs[1] = pop();
                regs[0] = pop();
                push(regs[0] <= regs[1] ? 1 : 0);
                break;
            }
            case OP_CMP_GT: {
                regs[1] = pop();
                regs[0] = pop();
                push(regs[0] > regs[1] ? 1 : 0);
                break;
            }
            case OP_CMP_GE: {
                regs[1] = pop();
                regs[0] = pop();
                push(regs[0] >= regs[1] ? 1 : 0);
                break;
            }
            case OP_JUMP: {
                ip = readU32();
                break;
            }
            case OP_JZ: {
                int64_t cond = pop();
                uint32_t addr = readU32();
                if(cond == 0) ip = addr;
                break;
            }
            case OP_JNZ: {
                int64_t cond = pop();
                uint32_t addr = readU32();
                if(cond != 0) ip = addr;
                break;
            }
            case OP_CALL: {
                uint32_t addr = readU32();
                push(static_cast<int64_t>(ip));
                ip = addr;
                break;
            }
            case OP_RET: {
                uint32_t argBytes = readU32();
                int64_t returnValue = pop();
                SP = FP;
                int64_t oldFp = pop();
                FP = static_cast<int32_t>(oldFp);
                int64_t returnAddress = pop();
                SP += argBytes / 8;
                if(returnAddress == -1){
                    return static_cast<int>(returnValue);
                }
                push(returnValue);
                ip = static_cast<size_t>(returnAddress);
                break;
            }
            case OP_PRINT: {
                int64_t value = pop();
                std::cout << value << std::endl;
                break;
            }
            case OP_ENTER: {
                uint32_t bytes = readU32();
                push(FP);
                FP = SP;
                SP -= static_cast<int32_t>(bytes / 8);
                break;
            }
            case OP_LEAVE: {
                readU32();
                SP = FP;
                int64_t oldFp = pop();
                FP = static_cast<int32_t>(oldFp);
                break;
            }
            case OP_POP: {
                pop();
                break;
            }
            default:
                throw std::runtime_error("Unknown opcode: " + std::to_string((int)op));
        }
    }
    return 0;
}
