#pragma once
#include "ast.h"
#include "instruction.h"
#include "SymbolTable.h"

class Compiler {
public:
    Executable compile(const Program& prog);

private:
    Executable  exe_;
    SymbolTable symbols_;

    void emitStmt    (const Stmt& s);
    void emitExpr    (const Expr& e);
    void emitBlock   (const BlockStmt& b);
    void emitFuncDecl(const FuncDecl& f);
    void emitIf      (const IfStmt& s);
    void emitWhile   (const WhileStmt& s);

    int  emit      (Instruction ins);
    int  currentIdx() const;
    void patchJump (int idx, int target);
};