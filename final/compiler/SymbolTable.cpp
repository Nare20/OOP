#include "SymbolTable.hpp"
#include <stdexcept>

SymbolTable::SymbolTable(){
    reset();
}

void SymbolTable::reset(){
    scopes.clear();
    scopes.emplace_back();
    nextLocalOffset = -8;
}

void SymbolTable::enterScope(){
    scopes.emplace_back();
}

void SymbolTable::exitScope(){
    if(scopes.size() <= 1) throw std::runtime_error("cannot exit global scope");
    scopes.pop_back();
}

const SymbolTable::Symbol* SymbolTable::declare(const std::string &name, Kind kind, int index){
    auto &scope = scopes.back();
    auto it = scope.find(name);
    if(it != scope.end()) return &it->second;

    Symbol symbol;
    symbol.kind = kind;
    if(kind == Kind::Variable){
        symbol.offset = nextLocalOffset;
        nextLocalOffset -= 8;
    } else if(kind == Kind::Parameter){
        if(index < 0) throw std::runtime_error("parameter index required for parameter symbol");
        symbol.index = index;
        symbol.offset = 16 + index * 8;
    } else {
        symbol.offset = -1;
    }

    auto inserted = scope.emplace(name, symbol);
    return &inserted.first->second;
}

const SymbolTable::Symbol* SymbolTable::lookup(const std::string &name) const{
    for(int i = (int)scopes.size() - 1; i >= 0; --i){
        auto it = scopes[i].find(name);
        if(it != scopes[i].end()) return &it->second;
    }
    return nullptr;
}

const std::unordered_map<std::string, SymbolTable::Symbol>& SymbolTable::currentScope() const{
    return scopes.back();
}
