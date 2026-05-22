#pragma once
#include <cstdint>
#include <vector>
#include <string>

enum Op : uint8_t {
    OP_HALT = 0,
    OP_PUSH_CONST = 1,
    OP_LOAD_VAR = 2,
    OP_STORE_VAR = 3,
    OP_ADD = 4,
    OP_SUB = 5,
    OP_MUL = 6,
    OP_DIV = 7,
    OP_CMP_EQ = 8,
    OP_CMP_NE = 9,
    OP_CMP_LT = 10,
    OP_CMP_LE = 11,
    OP_CMP_GT = 12,
    OP_CMP_GE = 13,
    OP_JUMP = 20,
    OP_JZ = 21,
    OP_JNZ = 22,
    OP_CALL = 30,
    OP_RET = 31,
    OP_PRINT = 40,
    OP_ENTER = 50,
    OP_LEAVE = 51,
    OP_POP = 52
};

std::vector<uint8_t> readBytecodeFile(const std::string &path);
