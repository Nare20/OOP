#pragma once
#include "ASTNode.hpp"
#include "BlockNode.hpp"
#include <string>
#include <vector>
#include <memory>

struct FuncNode : ASTNode {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<BlockNode> body;

    void dump(int indent, std::ostream &o) const override {
        o << std::string(indent, ' ') << "Func(" << name << ")\n";
        o << std::string(indent + 2, ' ') << "Params(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i) o << ",";
            o << params[i];
        }
        o << ")\n";
        if (body) body->dump(indent + 2, o);
    }
};
