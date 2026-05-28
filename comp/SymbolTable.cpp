#include "SymbolTable.h"
#include "utils.h"
#include <iostream>

void SymbolTable::enterScope() {
    scopes_.push_back({});
}

void SymbolTable::exitScope() {
    if (!scopes_.empty()) scopes_.pop_back();
}

int SymbolTable::declare(const std::string& name) {
    if (scopes_.empty()) scopes_.push_back({});
    SymbolInfo info;
    info.name       = name;
    info.scopeDepth = (int)scopes_.size() - 1;
    info.slot       = nextSlot_++;
    info.isFunc     = false;
    scopes_.back()[name] = info;
    return info.slot;
}

void SymbolTable::declareFunc(const std::string& name, int addr) {
    if (scopes_.empty()) scopes_.push_back({});
    SymbolInfo info;
    info.name       = name;
    info.scopeDepth = (int)scopes_.size() - 1;
    info.slot       = -1;
    info.isFunc     = true;
    info.funcAddr   = addr;
    scopes_.back()[name] = info;
}

const SymbolInfo& SymbolTable::resolve(const std::string& name) const {
    for (int i = (int)scopes_.size() - 1; i >= 0; --i) {
        auto it = scopes_[i].find(name);
        if (it != scopes_[i].end()) return it->second;
    }
    throw CompileError(0, "Undefined symbol: " + name);
}

bool SymbolTable::exists(const std::string& name) const {
    for (int i = (int)scopes_.size() - 1; i >= 0; --i)
        if (scopes_[i].count(name)) return true;
    return false;
}

void SymbolTable::patchFuncAddr(const std::string& name, int addr) {
    for (int i = (int)scopes_.size() - 1; i >= 0; --i) {
        auto it = scopes_[i].find(name);
        if (it != scopes_[i].end()) { it->second.funcAddr = addr; return; }
    }
}

void SymbolTable::dump() const {
    std::cout << "=== SymbolTable ===\n";
    for (int d = 0; d < (int)scopes_.size(); ++d) {
        std::cout << "  Scope " << d << ":\n";
        for (auto& [k, v] : scopes_[d])
            std::cout << "    " << k << (v.isFunc ? " [func addr=" + std::to_string(v.funcAddr) + "]"
                                                   : " [slot=" + std::to_string(v.slot) + "]") << "\n";
    }
}