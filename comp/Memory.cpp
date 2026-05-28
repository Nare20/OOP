#include "Memory.h"
#include "utils.h"
#include <iostream>
#include <cmath>
#include <sstream>

std::string valueToString(const Value& v) {
    if (std::holds_alternative<double>(v)) {
        double d = std::get<double>(v);
        if (d == std::floor(d) && !std::isinf(d))
            return std::to_string((long long)d);
        std::ostringstream oss; oss << d; return oss.str();
    }
    return std::get<std::string>(v);
}

bool valueToBool(const Value& v) {
    if (std::holds_alternative<double>(v)) return std::get<double>(v) != 0.0;
    return !std::get<std::string>(v).empty();
}

// ── Operand stack ─────────────────────────────────────────────────────────────
void Memory::push(Value v) { stack_.push_back(std::move(v)); }

Value Memory::pop() {
    if (stack_.empty()) throw RuntimeError(0, "Stack underflow");
    Value v = std::move(stack_.back()); stack_.pop_back(); return v;
}

Value& Memory::top() {
    if (stack_.empty()) throw RuntimeError(0, "Stack is empty");
    return stack_.back();
}

// ── Call frames ───────────────────────────────────────────────────────────────
void Memory::pushFrame(int retAddr) {
    frames_.push_back(Frame{retAddr, {}});
}

void Memory::popFrame() {
    if (frames_.empty()) throw RuntimeError(0, "No frame to pop");
    frames_.pop_back();
}

int Memory::returnAddr() const {
    if (frames_.empty()) throw RuntimeError(0, "No active frame");
    return frames_.back().returnAddr;
}

Frame& Memory::currentFrame() {
    if (frames_.empty()) throw RuntimeError(0, "No active frame");
    return frames_.back();
}

// ── Variable access ───────────────────────────────────────────────────────────
void Memory::store(const std::string& name, Value v) {
    if (!frames_.empty()) frames_.back().locals[name] = std::move(v);
    else                  globals_[name] = std::move(v);
}

Value Memory::load(const std::string& name) const {
    if (!frames_.empty()) {
        auto it = frames_.back().locals.find(name);
        if (it != frames_.back().locals.end()) return it->second;
    }
    auto it = globals_.find(name);
    if (it != globals_.end()) return it->second;
    throw RuntimeError(0, "Undefined variable: " + name);
}

bool Memory::varExists(const std::string& name) const {
    if (!frames_.empty() && frames_.back().locals.count(name)) return true;
    return globals_.count(name) > 0;
}

void Memory::reset() { stack_.clear(); frames_.clear(); globals_.clear(); }

void Memory::dumpStack() const {
    std::cout << "Stack [" << stack_.size() << "]: ";
    for (auto& v : stack_) std::cout << valueToString(v) << " ";
    std::cout << "\n";
}