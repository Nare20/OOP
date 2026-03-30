#pragma once
#include <vector>
#include "Node.hpp"

enum class OpCode {
    LOADI,
    LOAD,
    STORE,
    MOVE,

    ADD,
    SUB,
    MUL,
    DIV,

    HALT
};

struct Instruction {
    OpCode op;
    int a;
    int b;
};

class VM {
public:
    int R[8]{0};
    int PC = 0;

    std::vector<int> data;

    VM(int dataSize);

    void execute(const std::vector<Instruction>& code);
};