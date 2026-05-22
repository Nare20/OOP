#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "bytecode.hpp"

constexpr int REGISTER_COUNT = 8;
constexpr int MEMORY_WORDS = 4096;

class VM {
public:
    VM();
    bool load(const std::vector<uint8_t> &bytecode);
    int run();
private:
    int64_t regs[REGISTER_COUNT];
    int64_t memory[MEMORY_WORDS];
    int32_t SP = MEMORY_WORDS;
    int32_t FP = MEMORY_WORDS;
    size_t ip = 0;
    std::vector<uint8_t> code;

    void push(int64_t v);
    int64_t pop();
    uint8_t readU8();
    uint32_t readU32();
    int64_t readI64();
};
