#include "Compiler.hpp"



void Compiler::genBinary(Node* node, OpCode op) {
    gen(node->left);
    code.push_back({OpCode::MOVE, 1, 0}); // R1 = R0

    gen(node->right);
    code.push_back({OpCode::MOVE, 2, 0}); // R2 = R0

    code.push_back({OpCode::MOVE, 0, 1}); // R0 = R1
    code.push_back({op, 0, 2});           // R0 = R0 op R2
}

void Compiler::gen(Node* node) {
    if (!node) return;

    if (node->type == NodeType::NUM) {
        code.push_back({OpCode::LOADI, 0, node->value});
    }
    else if (node->type == NodeType::VAR) {
        int addr = sym.table[node->name];
        code.push_back({OpCode::LOAD, 0, addr});
    }
    else if (node->type == NodeType::ADD) genBinary(node, OpCode::ADD);
    else if (node->type == NodeType::SUB) genBinary(node, OpCode::SUB);
    else if (node->type == NodeType::MUL) genBinary(node, OpCode::MUL);
    else if (node->type == NodeType::DIV) genBinary(node, OpCode::DIV);

    else if (node->type == NodeType::ASSIGN) {
        gen(node->left);
        int addr = sym.table[node->name];
        code.push_back({OpCode::STORE, 0, addr});
    }
    else if (node->type == NodeType::PROGRAM) {
        for (auto child : node->children)
            gen(child);
    }
}

void Compiler::compile(const std::string& input) {
    root = parse(input);

    sym.table.clear();
    sym.build(root);

    code.clear();
    gen(root);

    code.push_back({OpCode::HALT, 0, 0});
}
