#include <map>
#include <vector>
#include <string>

enum class NodeType { NUM, VAR, ADD, SUB, MUL, DIV, ASSIGN, PROGRAM };

struct Node {
    NodeType type;
    int value = 0;
    std::string name;
    Node* left = nullptr;
    Node* right = nullptr;
    std::vector<Node*> children;
    
    Node(NodeType t) : type(t) {}
};


void collectSymbols(Node* node, std::map<std::string,int>& sym) {
    if (!node) return;

    if (node->type == NodeType::VAR || node->type == NodeType::ASSIGN) {
        if (!sym.count(node->name)) {
            sym[node->name] = sym.size(); 
        }
    }

    collectSymbols(node->left, sym);
    collectSymbols(node->right, sym);
    for (auto child : node->children)
        collectSymbols(child, sym);
}


std::vector<int> createDataVector(const std::map<std::string,int>& sym) {
    return std::vector<int>(sym.size(), 0);
}