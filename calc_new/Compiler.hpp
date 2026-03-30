#pragma once
#include <vector>
#include <string>
#include "Node.hpp"
#include "SymbolTable.hpp"
#include "VM.hpp"

Node* parse(const std::string& input);

class Compiler {
public:
    Node* root = nullptr;
    SymbolTable sym;
    std::vector<Instruction> code;

    void compile(const std::string& input);

private:
    void gen(Node* node);
    void genBinary(Node* node, OpCode op);
};