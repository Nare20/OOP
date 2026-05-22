#pragma once
#include "ASTNode.hpp"
#include "IR.hpp"

class IRGenerator {
public:
    IRProgram generate(const Program &program);
private:
    IRFunction *current = nullptr;
    void beginFunction(const FuncNode &fn);
    void finishFunction();
    void compileBlock(const BlockNode &block);
    void compileStmt(const Stmt *stmt);
    void compileExpr(const Expr *expr);
    int allocateVariable(const std::string &name);
    bool hasVariable(const std::string &name) const;
    void ensureParams(const FuncNode &fn);
    std::string uniqueLabel(const std::string &base);
    int labelCounter = 0;
};
