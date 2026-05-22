#pragma once
#include <memory>
#include <string>
#include <vector>
#include <iostream>

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void dump(int indent=0, std::ostream &o = std::cout) const = 0;
};

struct Expr : ASTNode {};

struct NumberExpr : Expr {
    int value;
    NumberExpr(int v): value(v) {}
    void dump(int indent, std::ostream &o) const override {
        o << std::string(indent,' ') << "Number(" << value << ")\n";
    }
};

struct VarExpr : Expr {
    std::string name;
    VarExpr(std::string n): name(std::move(n)){}
    void dump(int indent, std::ostream &o) const override { o << std::string(indent,' ') << "Var("<<name<<")\n"; }
};

struct BinaryExpr : Expr {
    std::string op; std::unique_ptr<Expr> left, right;
    BinaryExpr(std::string op_, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r): op(std::move(op_)), left(std::move(l)), right(std::move(r)){}
    void dump(int indent, std::ostream &o) const override {
        o << std::string(indent,' ') << "Binary("<<op<<")\n";
        left->dump(indent+2,o);
        right->dump(indent+2,o);
    }
};

struct CallExpr : Expr {
    std::string callee; std::vector<std::unique_ptr<Expr>> args;
    CallExpr(std::string c): callee(std::move(c)){}
    void dump(int indent, std::ostream &o) const override {
        o << std::string(indent,' ') << "Call("<<callee<<")\n";
        for(auto &a: args) a->dump(indent+2,o);
    }
};

struct Stmt : ASTNode {};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
    ExprStmt(std::unique_ptr<Expr> e): expr(std::move(e)){}
    void dump(int indent, std::ostream &o) const override { expr->dump(indent,o); }
};

struct AssignNode : Stmt {
    std::string name; std::unique_ptr<Expr> value;
    AssignNode(std::string n, std::unique_ptr<Expr> v): name(std::move(n)), value(std::move(v)){}
    void dump(int indent, std::ostream &o) const override { o<<std::string(indent,' ')<<"Assign("<<name<<")\n"; value->dump(indent+2,o); }
};

#include "ReturnNode.hpp"
#include "BlockNode.hpp"
#include "IfNode.hpp"
#include "WhileNode.hpp"
#include "FuncNode.hpp"

struct Program : ASTNode {
    std::vector<std::unique_ptr<FuncNode>> funcs;
    void dump(int indent, std::ostream &o) const override { o<<"Program\n"; for(auto &f: funcs) f->dump(2,o); }
};
