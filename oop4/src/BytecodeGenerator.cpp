#include "BytecodeGenerator.hpp"
#include <stdexcept>

void BytecodeGenerator::emitU8(uint8_t value){ code.push_back(value); }
void BytecodeGenerator::emitU32(uint32_t value){ for(int i = 0; i < 4; ++i) code.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF)); }
void BytecodeGenerator::emitI64(int64_t value){ for(int i = 0; i < 8; ++i) code.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF)); }

void BytecodeGenerator::emitLabel(const std::string &label){ labelOffsets[label] = static_cast<uint32_t>(code.size()); }

void BytecodeGenerator::emitLabelPatch(Op op, const std::string &label){
    emitU8(static_cast<uint8_t>(op));
    patches.emplace_back(static_cast<uint32_t>(code.size()), label);
    emitU32(0);
}

std::vector<uint8_t> BytecodeGenerator::generate(const IRProgram &program){
    code.clear();
    labelOffsets.clear();
    patches.clear();

    for(auto &fn : program.functions){
        emitLabel(fn.name);
        for(size_t i = 0; i < fn.insts.size(); ++i){
            const IRInst &inst = fn.insts[i];
            bool skipLeaveForRet = inst.op == IROp::LEAVE && i + 1 < fn.insts.size() && fn.insts[i + 1].op == IROp::RET;
            compileInst(inst, fn, skipLeaveForRet);
        }
    }

    patchLabels();
    return code;
}

void BytecodeGenerator::compileInst(const IRInst &inst, const IRFunction &fn, bool skipLeaveForRet){
    switch(inst.op){
        case IROp::LABEL:
            emitLabel(inst.label);
            break;
        case IROp::ENTER:
            emitU8(static_cast<uint8_t>(OP_ENTER));
            emitU32(static_cast<uint32_t>(inst.imm));
            break;
        case IROp::LEAVE:
            if(!skipLeaveForRet){
                emitU8(static_cast<uint8_t>(OP_LEAVE));
                emitU32(static_cast<uint32_t>(inst.imm));
            }
            break;
        case IROp::PUSH_CONST:
            emitU8(static_cast<uint8_t>(OP_PUSH_CONST));
            emitI64(inst.imm);
            break;
        case IROp::LOAD_VAR: {
            auto it = fn.varOffset.find(inst.name);
            if(it == fn.varOffset.end()) throw std::runtime_error("unknown variable in bytecode generation: " + inst.name);
            emitU8(static_cast<uint8_t>(OP_LOAD_VAR));
            emitU32(static_cast<uint32_t>(it->second));
            break;
        }
        case IROp::STORE_VAR: {
            auto it = fn.varOffset.find(inst.name);
            if(it == fn.varOffset.end()) throw std::runtime_error("unknown variable in bytecode generation: " + inst.name);
            emitU8(static_cast<uint8_t>(OP_STORE_VAR));
            emitU32(static_cast<uint32_t>(it->second));
            break;
        }
        case IROp::ADD: emitU8(static_cast<uint8_t>(OP_ADD)); break;
        case IROp::SUB: emitU8(static_cast<uint8_t>(OP_SUB)); break;
        case IROp::MUL: emitU8(static_cast<uint8_t>(OP_MUL)); break;
        case IROp::DIV: emitU8(static_cast<uint8_t>(OP_DIV)); break;
        case IROp::CMP_EQ: emitU8(static_cast<uint8_t>(OP_CMP_EQ)); break;
        case IROp::CMP_NE: emitU8(static_cast<uint8_t>(OP_CMP_NE)); break;
        case IROp::CMP_LT: emitU8(static_cast<uint8_t>(OP_CMP_LT)); break;
        case IROp::CMP_LE: emitU8(static_cast<uint8_t>(OP_CMP_LE)); break;
        case IROp::CMP_GT: emitU8(static_cast<uint8_t>(OP_CMP_GT)); break;
        case IROp::CMP_GE: emitU8(static_cast<uint8_t>(OP_CMP_GE)); break;
        case IROp::JUMP: emitLabelPatch(OP_JUMP, inst.label); break;
        case IROp::JZ: emitLabelPatch(OP_JZ, inst.label); break;
        case IROp::JNZ: emitLabelPatch(OP_JNZ, inst.label); break;
        case IROp::CALL: emitLabelPatch(OP_CALL, inst.label); break;
        case IROp::RET:
            emitU8(static_cast<uint8_t>(OP_RET));
            emitU32(static_cast<uint32_t>(inst.index));
            break;
        case IROp::PRINT: emitU8(static_cast<uint8_t>(OP_PRINT)); break;
        case IROp::POP: emitU8(static_cast<uint8_t>(OP_POP)); break;
        default:
            throw std::runtime_error("Unsupported IR opcode in bytecode generation");
    }
}

void BytecodeGenerator::patchLabels(){
    for(auto &patch : patches){
        uint32_t pos = patch.first;
        auto it = labelOffsets.find(patch.second);
        if(it == labelOffsets.end()) throw std::runtime_error("undefined label: " + patch.second);
        uint32_t addr = it->second;
        for(int i = 0; i < 4; ++i){
            code[pos + i] = static_cast<uint8_t>((addr >> (i * 8)) & 0xFF);
        }
    }
}
