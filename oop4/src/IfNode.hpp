#pragma once
#include "ASTNode.hpp"
#include "BlockNode.hpp"
#include <memory>
#include <string>
#include <iostream>

struct IfNode : Stmt {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<BlockNode> thenBlock;
    std::unique_ptr<BlockNode> elseBlock;

    void dump(int indent, std::ostream &o) const override {
        o << std::string(indent, ' ') << "If\n";
        cond->dump(indent + 2, o);
        thenBlock->dump(indent + 2, o);
        if (elseBlock) elseBlock->dump(indent + 2, o);
    }
};
