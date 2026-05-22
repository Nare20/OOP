#pragma once
#include "IR.hpp"
#include "bytecode.hpp"
#include <vector>
#include <string>
#include <unordered_map>

class BytecodeGenerator {
public:
    std::vector<uint8_t> generate(const IRProgram &program);

private:
    std::vector<uint8_t> code;
    std::unordered_map<std::string, uint32_t> labelOffsets;
    std::vector<std::pair<uint32_t, std::string>> patches;

    void emitU8(uint8_t value);
    void emitU32(uint32_t value);
    void emitI64(int64_t value);
    void emitLabel(const std::string &label);
    void emitLabelPatch(Op op, const std::string &label);
    void compileInst(const IRInst &inst, const IRFunction &fn, bool skipLeaveForRet);
    void patchLabels();
};
