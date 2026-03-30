#include "SymbolTable.hpp"

static void collect(Node* node, std::map<std::string,int>& sym) {
    if (!node) return;

    if (node->type == NodeType::VAR || node->type == NodeType::ASSIGN) {
        if (!sym.count(node->name))
            sym[node->name] = (int)sym.size();
    }

    collect(node->left, sym);
    collect(node->right, sym);

    for (auto child : node->children)
        collect(child, sym);
}

void SymbolTable::build(Node* root) {
    collect(root, table);
}