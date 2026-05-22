#include "Compiler.hpp"
#include "bytecode.hpp"
#include "SymbolTable.hpp"
#include "RegisterAllocator.hpp"
#include <cstring>
#include <stdexcept>
#include <fstream>

struct FnCompiler {
    const FuncNode *fn;
    std::vector<uint8_t> code;
    SymbolTable sym;
    RegisterAllocator regAlloc;
    std::vector<std::pair<size_t,std::string>> callPatches; // offset, name
    void emitU8(uint8_t v){ code.push_back(v); }
    void emitInt64(int64_t v){ uint8_t buf[8]; std::memcpy(buf,&v,8); code.insert(code.end(), buf, buf+8); }
    void emitU32(uint32_t v){ uint8_t buf[4]; std::memcpy(buf,&v,4); code.insert(code.end(), buf, buf+4); }
    void patchU32(size_t off, uint32_t v){ if(off+4>code.size()) throw std::runtime_error("patch out of range"); std::memcpy(&code[off], &v, 4); }

    void compileExpr(const Expr *e){
        if(auto n = dynamic_cast<const NumberExpr*>(e)){
            emitU8(OP_CONST); emitInt64(n->value);
            return;
        }
        if(auto v = dynamic_cast<const VarExpr*>(e)){
            auto symbol = sym.lookup(v->name);
            if(!symbol) throw std::runtime_error("unknown variable: "+v->name);
            int r = regAlloc.lookup(v->name);
            if(r < 0) r = regAlloc.allocate(v->name);
            emitU8(OP_LOAD); emitU8((uint8_t)r);
            return;
        }
        if(auto b = dynamic_cast<const BinaryExpr*>(e)){
            compileExpr(b->left.get()); compileExpr(b->right.get());
            if(b->op=="+") emitU8(OP_ADD);
            else if(b->op=="-") emitU8(OP_SUB);
            else if(b->op=="*") emitU8(OP_MUL);
            else if(b->op=="/") emitU8(OP_DIV);
            else throw std::runtime_error("unknown binary op");
            return;
        }
        if(auto c = dynamic_cast<const CallExpr*>(e)){
            // evaluate args left-to-right, then store into registers (reverse order)
            for(auto &arg: c->args) compileExpr(arg.get());
            size_t n = c->args.size();
            for(size_t i=0;i<n;i++){
                // store into register i, but must do in reverse: last arg stored first
            }
            // store in reverse
            for(size_t idx=0; idx<n; ++idx){ size_t i = n-1-idx; // target reg i
                emitU8(OP_STORE); emitU8((uint8_t)i);
            }
            // emit CALL with placeholder address
            emitU8(OP_CALL);
            size_t addrOff = code.size(); emitU32(0);
            callPatches.emplace_back(addrOff, c->callee);
            return;
        }
        throw std::runtime_error("unsupported expr node");
    }

    void compileStmt(const Stmt *s){
        if(auto es = dynamic_cast<const ExprStmt*>(s)){
            compileExpr(es->expr.get());
            // pop into scratch register to discard
            emitU8(OP_STORE); emitU8((uint8_t)(REGISTER_COUNT-1));
            return;
        }
        if(auto an = dynamic_cast<const AssignNode*>(s)){
            compileExpr(an->value.get());
            auto symbol = sym.lookup(an->name);
            if(!symbol){
                symbol = sym.declare(an->name, SymbolTable::Kind::Variable);
            }
            int r = regAlloc.lookup(an->name);
            if(r < 0) r = regAlloc.allocate(an->name);
            emitU8(OP_STORE); emitU8((uint8_t)r);
            return;
        }
        if(auto rn = dynamic_cast<const ReturnNode*>(s)){
            if(rn->value) compileExpr(rn->value.get()); else { emitU8(OP_CONST); emitInt64(0); }
            emitU8(OP_RET);
            return;
        }
        if(auto bn = dynamic_cast<const BlockNode*>(s)){
            sym.enterScope();
            for(auto &st: bn->stmts) compileStmt(st.get());
            for(auto &entry : sym.currentScope()) regAlloc.release(entry.first);
            sym.exitScope();
            return;
        }
        if(auto in = dynamic_cast<const IfNode*>(s)){
            compileExpr(in->cond.get());
            emitU8(OP_JZ); size_t jzOff = code.size(); emitU32(0);
            // then
            sym.enterScope();
            compileStmt(in->thenBlock.get());
            for(auto &entry : sym.currentScope()) regAlloc.release(entry.first);
            sym.exitScope();
            // jump over else
            emitU8(OP_JMP); size_t jmpOff = code.size(); emitU32(0);
            size_t elseAddr = code.size();
            // patch jz to elseAddr
            patchU32(jzOff, (uint32_t)elseAddr);
            if(in->elseBlock){
                sym.enterScope();
                compileStmt(in->elseBlock.get());
                for(auto &entry : sym.currentScope()) regAlloc.release(entry.first);
                sym.exitScope();
            }
            size_t after = code.size();
            patchU32(jmpOff, (uint32_t)after);
            return;
        }
        if(auto wn = dynamic_cast<const WhileNode*>(s)){
            size_t loopStart = code.size();
            compileExpr(wn->cond.get());
            emitU8(OP_JZ); size_t jzOff = code.size(); emitU32(0);
            sym.enterScope();
            compileStmt(wn->body.get());
            for(auto &entry : sym.currentScope()) regAlloc.release(entry.first);
            sym.exitScope();
            emitU8(OP_JMP); emitU32((uint32_t)loopStart);
            size_t after = code.size(); patchU32(jzOff, (uint32_t)after);
            return;
        }
        throw std::runtime_error("unsupported statement node");
    }

    void compileFunction(){
        // params are assigned to registers 0..n-1 by the caller.
        sym.reset();
        regAlloc.reset();
        sym.enterScope();
        // declare params and assign them to the first registers
        for(size_t i=0;i<fn->params.size();++i){
            sym.declare(fn->params[i], SymbolTable::Kind::Parameter, (int)i);
            regAlloc.allocate(fn->params[i]);
        }
        // compile body statements
        compileStmt(fn->body.get());
        // ensure function ends with a return
        emitU8(OP_CONST); emitInt64(0); emitU8(OP_RET);
        sym.exitScope();
    }
};

Compiler::Compiler(){}

std::vector<uint8_t> Compiler::compileProgram(const Program &p){
    // compile each function into its own buffer
    struct Buf { const FuncNode *fn; std::vector<uint8_t> code; std::vector<std::pair<size_t,std::string>> patches; };
    std::vector<Buf> bufs;
    for(auto &f: p.funcs){
        FnCompiler fc; fc.fn = f.get();
        fc.compileFunction();
        bufs.push_back({f.get(), std::move(fc.code), std::move(fc.callPatches)});
    }
    // assign addresses
    std::unordered_map<std::string,uint32_t> addrs;
    uint32_t base = 0;
    for(auto &b: bufs){ addrs[b.fn->name] = base; base += (uint32_t)b.code.size(); }
    // patch calls
    for(auto &b: bufs){ for(auto &cp: b.patches){ auto it = addrs.find(cp.second); if(it==addrs.end()) throw std::runtime_error("unknown function: "+cp.second); uint32_t addr = it->second; std::memcpy(&b.code[cp.first], &addr, 4); } }
    // concatenate
    std::vector<uint8_t> out;
    out.reserve(base);
    for(auto &b: bufs) { out.insert(out.end(), b.code.begin(), b.code.end()); }
    return out;
}

void Compiler::writeBytecode(const std::string &path, const std::vector<uint8_t> &bc){ std::ofstream out(path, std::ios::binary); out.write((const char*)bc.data(), bc.size()); }
