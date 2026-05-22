#pragma once
#include <string>
#include <unordered_map>
#include <vector>

class SymbolTable {
public:
    enum class Kind { Variable, Function, Parameter };

    struct Symbol {
        Kind kind = Kind::Variable;
        int offset = 0;   // stack offset for locals/parameters
        int index = -1;   // parameter index for parameter symbols
    };

    SymbolTable();
    void reset();
    void enterScope();
    void exitScope();
    const Symbol* declare(const std::string &name, Kind kind = Kind::Variable, int index = -1);
    const Symbol* lookup(const std::string &name) const;
    const std::unordered_map<std::string, Symbol>& currentScope() const;

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    int nextLocalOffset = -8;
};
