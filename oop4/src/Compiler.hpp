#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "ASTNode.hpp"

class Compiler {
public:
    Compiler();
    std::vector<uint8_t> compileProgram(const Program &p);
    void writeBytecode(const std::string &path, const std::vector<uint8_t> &bc);
private:
};
