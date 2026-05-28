#include "instruction.h"
#include <sstream>

std::string Instruction::toString() const {
    std::ostringstream o;
    switch (op) {
        case OpCode::PUSH_NUM:      o << "PUSH_NUM  " << std::get<double>(operand); break;
        case OpCode::PUSH_STR:      o << "PUSH_STR  \"" << std::get<std::string>(operand) << '"'; break;
        case OpCode::POP:           o << "POP"; break;
        case OpCode::LOAD:          o << "LOAD      " << std::get<std::string>(operand); break;
        case OpCode::STORE:         o << "STORE     " << std::get<std::string>(operand); break;
        case OpCode::ADD:           o << "ADD"; break;
        case OpCode::SUB:           o << "SUB"; break;
        case OpCode::MUL:           o << "MUL"; break;
        case OpCode::DIV:           o << "DIV"; break;
        case OpCode::MOD:           o << "MOD"; break;
        case OpCode::EQ:            o << "EQ"; break;
        case OpCode::NEQ:           o << "NEQ"; break;
        case OpCode::LT:            o << "LT"; break;
        case OpCode::LTE:           o << "LTE"; break;
        case OpCode::GT:            o << "GT"; break;
        case OpCode::GTE:           o << "GTE"; break;
        case OpCode::AND:           o << "AND"; break;
        case OpCode::OR:            o << "OR"; break;
        case OpCode::NOT:           o << "NOT"; break;
        case OpCode::JUMP:          o << "JUMP      " << std::get<int>(operand); break;
        case OpCode::JUMP_IF_FALSE: o << "JIF_FALSE " << std::get<int>(operand); break;
        case OpCode::CALL:          o << "CALL      " << std::get<std::string>(operand); break;
        case OpCode::RETURN:        o << "RETURN"; break;
        case OpCode::PRINT:         o << "PRINT"; break;
        case OpCode::HALT:          o << "HALT"; break;
    }
    return o.str();
}