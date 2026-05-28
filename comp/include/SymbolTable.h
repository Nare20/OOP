#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

struct SymbolInfo {
    std::string name;
    int         scopeDepth = 0;
    int         slot       = 0;
    bool        isFunc     = false;
    int         funcAddr   = -1;
};

class SymbolTable {
public:
    void enterScope();
    void exitScope();
    int  declare    (const std::string& name);
    void declareFunc(const std::string& name, int addr);
    const SymbolInfo& resolve(const std::string& name) const;
    bool exists     (const std::string& name) const;
    void patchFuncAddr(const std::string& name, int addr);
    void dump() const;

private:
    std::vector<std::unordered_map<std::string, SymbolInfo>> scopes_;
    int nextSlot_ = 0;
};