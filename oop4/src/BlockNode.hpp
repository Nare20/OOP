#pragma once
#include "ASTNode.hpp"
#include <vector>
#include <memory>

struct BlockNode : Stmt {
    std::vector<std::unique_ptr<Stmt>> stmts;
    void dump(int indent, std::ostream &o) const override {
        o << std::string(indent, ' ') << "Block\n";
        for (auto &s : stmts) s->dump(indent + 2, o);
    }
};
