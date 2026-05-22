#pragma once
#include <array>
#include <string>
#include <unordered_map>

class RegisterAllocator {
public:
    RegisterAllocator();
    void reset();
    int allocate(const std::string &name);
    int allocateTemp();
    int lookup(const std::string &name) const;
    bool hasRegister(const std::string &name) const;
    void release(const std::string &name);
    void freeRegister(int reg);

private:
    int allocateRegister();
    std::array<bool, 8> used{};
    std::unordered_map<std::string, int> mapping;
    int tempCounter = 0;
};
