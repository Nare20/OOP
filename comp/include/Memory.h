#pragma once
#include <string>
#include <variant>
#include <unordered_map>
#include <vector>
#include <stdexcept>

using Value = std::variant<double, std::string>;

std::string valueToString(const Value& v);
bool        valueToBool  (const Value& v);

struct Frame {
    int returnAddr;
    std::unordered_map<std::string, Value> locals;
};

class Memory {
public:
    void   push(Value v);
    Value  pop();
    Value& top();
    bool   stackEmpty() const { return stack_.empty(); }
    size_t stackSize()  const { return stack_.size(); }

    void   pushFrame(int returnAddr);
    void   popFrame();
    bool   inFunction() const { return !frames_.empty(); }
    int    returnAddr() const;
    Frame& currentFrame();

    void  store(const std::string& name, Value v);
    Value load (const std::string& name) const;
    bool  varExists(const std::string& name) const;

    void reset();
    void dumpStack() const;

private:
    std::vector<Value>                     stack_;
    std::vector<Frame>                     frames_;
    std::unordered_map<std::string, Value> globals_;
};