#pragma once
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

enum class OpCode {
    PUSH_NUM, PUSH_STR, POP,
    LOAD, STORE,
    ADD, SUB, MUL, DIV, MOD,
    EQ, NEQ, LT, LTE, GT, GTE,
    AND, OR, NOT,
    JUMP, JUMP_IF_FALSE,
    CALL, RETURN,
    PRINT,
    HALT
};

using Operand = std::variant<std::monostate, double, std::string, int>;

struct Instruction {
    OpCode  op;
    Operand operand = std::monostate{};

    static Instruction make   (OpCode o)                   { return {o}; }
    static Instruction makeNum(OpCode o, double v)         { return {o, v}; }
    static Instruction makeStr(OpCode o, std::string s)    { return {o, std::move(s)}; }
    static Instruction makeInt(OpCode o, int v)            { return {o, v}; }

    std::string toString() const;
};

struct Executable {
    std::vector<Instruction>             code;
    std::unordered_map<std::string, int> functions;
};