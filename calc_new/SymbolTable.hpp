#pragma once
#include <map>
#include <string>
#include "Node.hpp"

class SymbolTable {
public:
    std::map<std::string, int> table;

    void build(Node* root);
};