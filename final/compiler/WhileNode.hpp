#pragma once
#include "ASTNode.hpp"
#include "BlockNode.hpp"
#include <memory>
#include <string>
#include <iostream>

struct WhileNode : Stmt {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<BlockNode> body;

    void dump(int indent, std::ostream &o) const override {
        o << std::string(indent, ' ') << "While\n";
        cond->dump(indent + 2, o);
        body->dump(indent + 2, o);
    }
};
