#include "CPU.h"
#include "utils.h"
#include <iostream>
#include <cmath>

Value CPU::applyBinaryOp(OpCode op, const Value& a, const Value& b) {
    auto getNum = [](const Value& v, const std::string& side) -> double {
        if (!std::holds_alternative<double>(v))
            throw RuntimeError(0, "Expected number on " + side);
        return std::get<double>(v);
    };

    switch (op) {
        case OpCode::ADD:
            if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b))
                return std::get<double>(a) + std::get<double>(b);
            return valueToString(a) + valueToString(b); // string concat
        case OpCode::SUB:  return getNum(a,"left") - getNum(b,"right");
        case OpCode::MUL:  return getNum(a,"left") * getNum(b,"right");
        case OpCode::DIV: {
            double db = getNum(b,"right");
            if (db == 0.0) throw RuntimeError(0, "Division by zero");
            return getNum(a,"left") / db;
        }
        case OpCode::MOD: {
            double db = getNum(b,"right");
            if (db == 0.0) throw RuntimeError(0, "Modulo by zero");
            return std::fmod(getNum(a,"left"), db);
        }
        case OpCode::EQ:  return (double)(valueToString(a) == valueToString(b));
        case OpCode::NEQ: return (double)(valueToString(a) != valueToString(b));
        case OpCode::LT:  return (double)(getNum(a,"left") <  getNum(b,"right"));
        case OpCode::LTE: return (double)(getNum(a,"left") <= getNum(b,"right"));
        case OpCode::GT:  return (double)(getNum(a,"left") >  getNum(b,"right"));
        case OpCode::GTE: return (double)(getNum(a,"left") >= getNum(b,"right"));
        case OpCode::AND: return (double)(valueToBool(a) && valueToBool(b));
        case OpCode::OR:  return (double)(valueToBool(a) || valueToBool(b));
        default: throw RuntimeError(0, "Unknown binary op");
    }
}

bool CPU::step(const Executable& exe, int& ip, Memory& mem) {
    if (ip >= (int)exe.code.size()) return false;
    const Instruction& ins = exe.code[ip++];

    switch (ins.op) {
        case OpCode::PUSH_NUM: mem.push(std::get<double>(ins.operand)); break;
        case OpCode::PUSH_STR: mem.push(std::get<std::string>(ins.operand)); break;
        case OpCode::POP:      mem.pop(); break;

        case OpCode::LOAD:  mem.push(mem.load(std::get<std::string>(ins.operand))); break;
        case OpCode::STORE: { Value v = mem.pop(); mem.store(std::get<std::string>(ins.operand), std::move(v)); break; }

        case OpCode::ADD: case OpCode::SUB: case OpCode::MUL:
        case OpCode::DIV: case OpCode::MOD:
        case OpCode::EQ:  case OpCode::NEQ:
        case OpCode::LT:  case OpCode::LTE:
        case OpCode::GT:  case OpCode::GTE:
        case OpCode::AND: case OpCode::OR: {
            Value b = mem.pop(), a = mem.pop();
            mem.push(applyBinaryOp(ins.op, a, b));
            break;
        }
        case OpCode::NOT: { Value v = mem.pop(); mem.push((double)(!valueToBool(v))); break; }

        case OpCode::JUMP:          ip = std::get<int>(ins.operand); break;
        case OpCode::JUMP_IF_FALSE: { Value c = mem.pop(); if (!valueToBool(c)) ip = std::get<int>(ins.operand); break; }

        case OpCode::CALL: {
            const std::string& fname = std::get<std::string>(ins.operand);
            auto it = exe.functions.find(fname);
            if (it == exe.functions.end()) throw RuntimeError(0, "Undefined function: " + fname);
            mem.pushFrame(ip);
            ip = it->second;
            break;
        }
        case OpCode::RETURN: {
            Value ret = mem.pop();
            int retAddr = mem.returnAddr();
            mem.popFrame();
            mem.push(std::move(ret));
            ip = retAddr;
            break;
        }
        case OpCode::PRINT: std::cout << valueToString(mem.pop()) << "\n"; break;
        case OpCode::HALT:  return false;
    }
    return true;
}