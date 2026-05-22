#include "IRGenerator.hpp"
#include <stdexcept>

IRProgram IRGenerator::generate(const Program &program){
    IRProgram out;
    for(auto &fn : program.funcs){
        beginFunction(*fn);
        compileBlock(*fn->body);
        int localBytes = current->localCount * 8;
        if(current->insts.size() > 1 && current->insts[1].op == IROp::ENTER){
            current->insts[1].imm = localBytes;
        }
        if(current->insts.empty() || current->insts.back().op != IROp::RET){
            IRInst constZero; constZero.op = IROp::PUSH_CONST; constZero.imm = 0;
            current->insts.push_back(constZero);
            current->insts.push_back({IROp::LEAVE, {}, {}, 0, localBytes});
            IRInst retInst; retInst.op = IROp::RET; retInst.index = (int)current->params.size() * 8;
            current->insts.push_back(retInst);
        }
        out.functions.push_back(std::move(*current));
        finishFunction();
    }
    return out;
}

void IRGenerator::beginFunction(const FuncNode &fn){
    current = new IRFunction();
    current->name = fn.name;
    current->params = fn.params;
    current->localCount = 0;
    current->insts.push_back({IROp::LABEL, {}, fn.name, 0, 0});
    current->insts.push_back({IROp::ENTER, {}, {}, 0, 0});
    ensureParams(fn);
}

void IRGenerator::finishFunction(){
    delete current;
    current = nullptr;
}

void IRGenerator::compileBlock(const BlockNode &block){
    for(auto &stmt : block.stmts) compileStmt(stmt.get());
}

void IRGenerator::compileStmt(const Stmt *stmt){
    if(auto assign = dynamic_cast<const AssignNode*>(stmt)){
        compileExpr(assign->value.get());
        if(!hasVariable(assign->name)) allocateVariable(assign->name);
        current->insts.push_back({IROp::STORE_VAR, assign->name, {}, 0, 0});
        return;
    }
    if(auto ret = dynamic_cast<const ReturnNode*>(stmt)){
        if(ret->value) compileExpr(ret->value.get()); else current->insts.push_back({IROp::PUSH_CONST, {}, {}, 0, 0});
        current->insts.push_back({IROp::LEAVE, {}, {}, 0, current->localCount * 8});
        IRInst retInst; retInst.op = IROp::RET; retInst.index = (int)current->params.size() * 8;
        current->insts.push_back(retInst);
        return;
    }
    if(auto exprStmt = dynamic_cast<const ExprStmt*>(stmt)){
        if(auto call = dynamic_cast<const CallExpr*>(exprStmt->expr.get()); call && call->callee == "print"){
            compileExpr(exprStmt->expr.get());
            return;
        }
        compileExpr(exprStmt->expr.get());
        current->insts.push_back({IROp::POP, {}, {}, 0, 0});
        return;
    }
    if(auto ifStmt = dynamic_cast<const IfNode*>(stmt)){
        compileExpr(ifStmt->cond.get());
        std::string elseLabel = uniqueLabel("else");
        std::string endLabel = uniqueLabel("endif");
        current->insts.push_back({IROp::JZ, {}, elseLabel, 0, 0});
        compileBlock(*ifStmt->thenBlock);
        current->insts.push_back({IROp::JUMP, {}, endLabel, 0, 0});
        current->insts.push_back({IROp::LABEL, {}, elseLabel, 0, 0});
        if(ifStmt->elseBlock) compileBlock(*ifStmt->elseBlock);
        current->insts.push_back({IROp::LABEL, {}, endLabel, 0, 0});
        return;
    }
    if(auto whileStmt = dynamic_cast<const WhileNode*>(stmt)){
        std::string startLabel = uniqueLabel("while_start");
        std::string endLabel = uniqueLabel("while_end");
        current->insts.push_back({IROp::LABEL, {}, startLabel, 0, 0});
        compileExpr(whileStmt->cond.get());
        current->insts.push_back({IROp::JZ, {}, endLabel, 0, 0});
        compileBlock(*whileStmt->body);
        current->insts.push_back({IROp::JUMP, {}, startLabel, 0, 0});
        current->insts.push_back({IROp::LABEL, {}, endLabel, 0, 0});
        return;
    }
    if(auto blockStmt = dynamic_cast<const BlockNode*>(stmt)){
        compileBlock(*blockStmt);
        return;
    }
    throw std::runtime_error("Unsupported statement in IR generation");
}

void IRGenerator::compileExpr(const Expr *expr){
    if(auto num = dynamic_cast<const NumberExpr*>(expr)){
        IRInst inst; inst.op = IROp::PUSH_CONST; inst.imm = num->value; current->insts.push_back(inst); return;
    }
    if(auto var = dynamic_cast<const VarExpr*>(expr)){
        if(!hasVariable(var->name)) throw std::runtime_error("Undefined variable: " + var->name);
        IRInst inst; inst.op = IROp::LOAD_VAR; inst.name = var->name; current->insts.push_back(inst); return;
    }
    if(auto bin = dynamic_cast<const BinaryExpr*>(expr)){
        compileExpr(bin->left.get()); compileExpr(bin->right.get());
        IRInst inst;
        if(bin->op == "+") inst.op = IROp::ADD;
        else if(bin->op == "-") inst.op = IROp::SUB;
        else if(bin->op == "*") inst.op = IROp::MUL;
        else if(bin->op == "/") inst.op = IROp::DIV;
        else if(bin->op == "==") inst.op = IROp::CMP_EQ;
        else if(bin->op == "!=") inst.op = IROp::CMP_NE;
        else if(bin->op == "<") inst.op = IROp::CMP_LT;
        else if(bin->op == "<=") inst.op = IROp::CMP_LE;
        else if(bin->op == ">") inst.op = IROp::CMP_GT;
        else if(bin->op == ">=") inst.op = IROp::CMP_GE;
        else throw std::runtime_error("Unknown binary operator: " + bin->op);
        current->insts.push_back(inst);
        return;
    }
    if(auto call = dynamic_cast<const CallExpr*>(expr)){
        if(call->callee == "print"){
            if(call->args.size() != 1) throw std::runtime_error("print expects one argument");
            compileExpr(call->args[0].get());
            current->insts.push_back({IROp::PRINT, {}, {}, 0, 0});
            return;
        }
        for(auto &arg : call->args) compileExpr(arg.get());
        IRInst inst; inst.op = IROp::CALL; inst.label = call->callee; inst.index = (int)call->args.size(); current->insts.push_back(inst);
        return;
    }
    throw std::runtime_error("Unsupported expression in IR generation");
}

int IRGenerator::allocateVariable(const std::string &name){
    if(hasVariable(name)) return current->varOffset[name];
    int offset = -(int)((current->localCount + 1) * 8);
    current->varOffset[name] = offset;
    current->localCount += 1;
    return offset;
}

bool IRGenerator::hasVariable(const std::string &name) const{
    return current->varOffset.find(name) != current->varOffset.end();
}

void IRGenerator::ensureParams(const FuncNode &fn){
    for(int i = 0; i < (int)fn.params.size(); ++i){
        current->varOffset[fn.params[i]] = 16 + i * 8;
    }
}

std::string IRGenerator::uniqueLabel(const std::string &base){
    return base + "_" + std::to_string(labelCounter++);
}
