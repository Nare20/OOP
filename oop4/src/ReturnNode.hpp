#pragma once
#include "ASTNode.hpp"
#include <memory>

struct ReturnNode : Stmt {
    std::unique_ptr<Expr> value;
    ReturnNode(std::unique_ptr<Expr> v): value(std::move(v)){}
    void dump(int indent, std::ostream &o) const override {
        o << std::string(indent, ' ') << "Return\n";
        if (value) value->dump(indent + 2, o);
    }
};
