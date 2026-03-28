#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

enum class NodeType { NUM, VAR, ADD, SUB, MUL, DIV, ASSIGN, PROGRAM };

struct Node {
    NodeType type;
    int value;
    string name;
    Node* left;
    Node* right;
    vector<Node*> children;

    Node(NodeType t) : type(t), value(0), left(nullptr), right(nullptr) {}
};

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

void gen(Node* node, vector<Instruction>& code, map<string,int>& sym);

void genBinary(Node* node, vector<Instruction>& code, map<string,int>& sym, OpCode op) {
    gen(node->left, code, sym);
    code.push_back({OpCode::MOVE, 1, 0});  // R1 = R0

    gen(node->right, code, sym);
    code.push_back({OpCode::MOVE, 2, 0});  // R2 = R0

    code.push_back({OpCode::MOVE, 0, 1});  // R0 = R1
    code.push_back({op, 0, 2});            // R0 = R0 op R2
}

void gen(Node* node, vector<Instruction>& code, map<string,int>& sym) {
    if (!node) return;

    if (node->type == NodeType::NUM) {
        code.push_back({OpCode::LOADI, 0, node->value});
    }
    else if (node->type == NodeType::VAR) {
        int addr = sym[node->name];
        code.push_back({OpCode::LOAD, 0, addr});
    }
    else if (node->type == NodeType::ADD) genBinary(node, code, sym, OpCode::ADD);
    else if (node->type == NodeType::SUB) genBinary(node, code, sym, OpCode::SUB);
    else if (node->type == NodeType::MUL) genBinary(node, code, sym, OpCode::MUL);
    else if (node->type == NodeType::DIV) genBinary(node, code, sym, OpCode::DIV);

    else if (node->type == NodeType::ASSIGN) {
        gen(node->left, code, sym);
        int addr = sym[node->name];
        code.push_back({OpCode::STORE, 0, addr});
    }
    else if (node->type == NodeType::PROGRAM) {
        for (auto child : node->children)
            gen(child, code, sym);
    }
}

int main() {
    
    Node* prog = new Node(NodeType::PROGRAM);

    Node* assignA = new Node(NodeType::ASSIGN);
    assignA->name = "a";
    assignA->left = new Node(NodeType::NUM);
    assignA->left->value = 5;

    Node* assignB = new Node(NodeType::ASSIGN);
    assignB->name = "b";
    assignB->left = new Node(NodeType::NUM);
    assignB->left->value = 2;

    Node* mul = new Node(NodeType::MUL);
    mul->left = new Node(NodeType::VAR);
    mul->left->name = "b";
    mul->right = new Node(NodeType::NUM);
    mul->right->value = 3;

    Node* add = new Node(NodeType::ADD);
    add->left = new Node(NodeType::VAR);
    add->left->name = "a";
    add->right = mul;

    prog->children.push_back(assignA);
    prog->children.push_back(assignB);
    prog->children.push_back(add);

    map<string,int> sym;
    sym["a"] = 0;
    sym["b"] = 1;

    vector<Instruction> code;
    gen(prog, code, sym);
    code.push_back({OpCode::HALT, 0, 0});

    cout << "Generated instructions:\n";
    for (int i = 0; i < (int)code.size(); i++) {
        cout << i << ": " << (int)code[i].op << " " << code[i].a << " " << code[i].b << endl;
    }
}