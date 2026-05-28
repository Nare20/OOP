#include "compiler_core.h"
#include "utils.h"

Executable Compiler::compile(const Program& prog) {
    exe_ = Executable{};
    symbols_.enterScope();

    // Forward-declare all top-level functions
    for (auto& s : prog.stmts)
        if (s->kind == Stmt::Kind::FuncDecl)
            symbols_.declareFunc(s->funcDecl.name, -1);

    for (auto& s : prog.stmts) emitStmt(*s);
    emit(Instruction::make(OpCode::HALT));

    symbols_.exitScope();
    return std::move(exe_);
}

int Compiler::emit(Instruction ins) {
    exe_.code.push_back(std::move(ins));
    return (int)exe_.code.size() - 1;
}
int Compiler::currentIdx() const { return (int)exe_.code.size(); }
void Compiler::patchJump(int idx, int target) {
    exe_.code[idx].operand = target;
}

void Compiler::emitStmt(const Stmt& s) {
    switch (s.kind) {
        case Stmt::Kind::Expr:
            emitExpr(*s.expr.expr);
            emit(Instruction::make(OpCode::POP));
            break;
        case Stmt::Kind::VarDecl:
            emitExpr(*s.varDecl.init);
            emit(Instruction::makeStr(OpCode::STORE, s.varDecl.name));
            break;
        case Stmt::Kind::Print:
            emitExpr(*s.print.value);
            emit(Instruction::make(OpCode::PRINT));
            break;
        case Stmt::Kind::Return:
            emitExpr(*s.ret.value);
            emit(Instruction::make(OpCode::RETURN));
            break;
        case Stmt::Kind::Block:  emitBlock(s.block);   break;
        case Stmt::Kind::If:     emitIf(s.ifStmt);     break;
        case Stmt::Kind::While:  emitWhile(s.whileStmt); break;
        case Stmt::Kind::FuncDecl: emitFuncDecl(s.funcDecl); break;
    }
}

void Compiler::emitBlock(const BlockStmt& b) {
    for (auto& s : b.stmts) emitStmt(*s);
}

void Compiler::emitFuncDecl(const FuncDecl& f) {
    int skip = emit(Instruction::makeInt(OpCode::JUMP, 0));
    int entry = currentIdx();
    exe_.functions[f.name] = entry;
    symbols_.patchFuncAddr(f.name, entry);

    for (int i = (int)f.params.size() - 1; i >= 0; --i)
        emit(Instruction::makeStr(OpCode::STORE, f.params[i]));

    emitBlock(*f.body);
    emit(Instruction::makeNum(OpCode::PUSH_NUM, 0.0));
    emit(Instruction::make(OpCode::RETURN));
    patchJump(skip, currentIdx());
}

void Compiler::emitIf(const IfStmt& s) {
    emitExpr(*s.cond);
    int jf = emit(Instruction::makeInt(OpCode::JUMP_IF_FALSE, 0));
    emitBlock(*s.thenBranch);
    if (s.elseBranch) {
        int je = emit(Instruction::makeInt(OpCode::JUMP, 0));
        patchJump(jf, currentIdx());
        emitBlock(*s.elseBranch);
        patchJump(je, currentIdx());
    } else {
        patchJump(jf, currentIdx());
    }
}

void Compiler::emitWhile(const WhileStmt& s) {
    int start = currentIdx();
    emitExpr(*s.cond);
    int jf = emit(Instruction::makeInt(OpCode::JUMP_IF_FALSE, 0));
    emitBlock(*s.body);
    emit(Instruction::makeInt(OpCode::JUMP, start));
    patchJump(jf, currentIdx());
}

void Compiler::emitExpr(const Expr& e) {
    switch (e.kind) {
        case Expr::Kind::Number:
            emit(Instruction::makeNum(OpCode::PUSH_NUM, e.number.value)); break;
        case Expr::Kind::String:
            emit(Instruction::makeStr(OpCode::PUSH_STR, e.string.value)); break;
        case Expr::Kind::Var:
            emit(Instruction::makeStr(OpCode::LOAD, e.var.name)); break;
        case Expr::Kind::Assign:
            emitExpr(*e.assign.value);
            emit(Instruction::makeStr(OpCode::STORE, e.assign.name));
            emit(Instruction::makeStr(OpCode::LOAD,  e.assign.name));
            break;
        case Expr::Kind::Binary:
            emitExpr(*e.binary.left);
            emitExpr(*e.binary.right);
            {
                const std::string& op = e.binary.op;
                if      (op=="+")  emit(Instruction::make(OpCode::ADD));
                else if (op=="-")  emit(Instruction::make(OpCode::SUB));
                else if (op=="*")  emit(Instruction::make(OpCode::MUL));
                else if (op=="/")  emit(Instruction::make(OpCode::DIV));
                else if (op=="%")  emit(Instruction::make(OpCode::MOD));
                else if (op=="==") emit(Instruction::make(OpCode::EQ));
                else if (op=="!=") emit(Instruction::make(OpCode::NEQ));
                else if (op=="<")  emit(Instruction::make(OpCode::LT));
                else if (op=="<=") emit(Instruction::make(OpCode::LTE));
                else if (op==">")  emit(Instruction::make(OpCode::GT));
                else if (op==">=") emit(Instruction::make(OpCode::GTE));
                else if (op=="&&") emit(Instruction::make(OpCode::AND));
                else if (op=="||") emit(Instruction::make(OpCode::OR));
                else throw CompileError(0, "Unknown operator: " + op);
            }
            break;
        case Expr::Kind::Unary:
            emitExpr(*e.unary.operand);
            if      (e.unary.op=="!") emit(Instruction::make(OpCode::NOT));
            else if (e.unary.op=="-") {
                emit(Instruction::makeNum(OpCode::PUSH_NUM, -1.0));
                emit(Instruction::make(OpCode::MUL));
            }
            break;
        case Expr::Kind::Call:
            for (auto& arg : e.call.args) emitExpr(*arg);
            emit(Instruction::makeStr(OpCode::CALL, e.call.callee));
            break;
    }
}