#pragma once
#include <string>
#include <vector>

enum class NodeType {
    NUM,
    VAR,

    ADD,
    SUB,
    MUL,
    DIV,

    ASSIGN,
    PROGRAM
};

struct Node {
    NodeType type;

    int value = 0;                  
    std::string name;               

    Node* left = nullptr;
    Node* right = nullptr;

    std::vector<Node*> children;    

    Node(NodeType t) : type(t) {}
};