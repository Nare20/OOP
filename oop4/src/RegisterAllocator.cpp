#include "RegisterAllocator.hpp"
#include <stdexcept>

RegisterAllocator::RegisterAllocator(){
    reset();
}

void RegisterAllocator::reset(){
    used.fill(false);
    mapping.clear();
    tempCounter = 0;
}

int RegisterAllocator::allocate(const std::string &name){
    auto it = mapping.find(name);
    if(it != mapping.end()) return it->second;
    int reg = allocateRegister();
    mapping[name] = reg;
    return reg;
}

int RegisterAllocator::allocateTemp(){
    std::string tempName = "__tmp" + std::to_string(tempCounter++);
    return allocate(tempName);
}

int RegisterAllocator::lookup(const std::string &name) const{
    auto it = mapping.find(name);
    return it == mapping.end() ? -1 : it->second;
}

bool RegisterAllocator::hasRegister(const std::string &name) const{
    return lookup(name) >= 0;
}

void RegisterAllocator::release(const std::string &name){
    auto it = mapping.find(name);
    if(it == mapping.end()) return;
    if(it->second >= 0 && it->second < (int)used.size()){
        used[it->second] = false;
    }
    mapping.erase(it);
}

void RegisterAllocator::freeRegister(int reg){
    if(reg < 0 || reg >= (int)used.size()) return;
    used[reg] = false;
    for(auto it = mapping.begin(); it != mapping.end(); ++it){
        if(it->second == reg){
            mapping.erase(it);
            break;
        }
    }
}

int RegisterAllocator::allocateRegister(){
    for(int i = 0; i < (int)used.size(); ++i){
        if(!used[i]){
            used[i] = true;
            return i;
        }
    }
    throw std::runtime_error("out of virtual registers");
}
