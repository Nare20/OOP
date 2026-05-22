#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

enum class IROp {
    NOP,
    LABEL,
    PUSH_CONST,
    LOAD_VAR,
    STORE_VAR,
    ADD,
    SUB,
    MUL,
    DIV,
    CMP_EQ,
    CMP_NE,
    CMP_LT,
    CMP_LE,
    CMP_GT,
    CMP_GE,
    JUMP,
    JZ,
    JNZ,
    CALL,
    RET,
    PRINT,
    ENTER,
    LEAVE,
    POP
};

struct IRInst {
    IROp op = IROp::NOP;
    std::string name;
    std::string label;
    int index = 0;
    int64_t imm = 0;
};

struct IRFunction {
    std::string name;
    std::vector<std::string> params;
    std::vector<IRInst> insts;
    int localCount = 0;
    std::unordered_map<std::string,int> varOffset;
};

struct IRProgram {
    std::vector<IRFunction> functions;
};
