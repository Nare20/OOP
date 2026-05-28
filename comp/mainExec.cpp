#include "instruction.h"
#include "VM.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Deserialize Executable from the text format written by mainCompiler
static Executable loadExe(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("Cannot open: " + path);

    Executable exe;
    std::string line;

    // Functions
    int nFuncs = 0;
    f >> line >> nFuncs; std::getline(f, line); // consume rest of line
    for (int i = 0; i < nFuncs; ++i) {
        std::string name; int addr;
        f >> name >> addr; std::getline(f, line);
        exe.functions[name] = addr;
    }

    // Code
    int nCode = 0;
    f >> line >> nCode; std::getline(f, line);

    // Re-parse each instruction line from its toString() representation
    auto parseIns = [](const std::string& s) -> Instruction {
        std::istringstream ss(s);
        std::string op; ss >> op;

        if (op == "PUSH_NUM") { double v; ss >> v; return Instruction::makeNum(OpCode::PUSH_NUM, v); }
        if (op == "PUSH_STR") { std::string rest; std::getline(ss, rest);
            // strip leading space and quotes
            size_t a = rest.find('"'), b = rest.rfind('"');
            std::string val = (a != std::string::npos && b > a) ? rest.substr(a+1, b-a-1) : trim(rest);
            return Instruction::makeStr(OpCode::PUSH_STR, val); }
        if (op == "LOAD")  { std::string n; ss >> n; return Instruction::makeStr(OpCode::LOAD,  n); }
        if (op == "STORE") { std::string n; ss >> n; return Instruction::makeStr(OpCode::STORE, n); }
        if (op == "CALL")  { std::string n; ss >> n; return Instruction::makeStr(OpCode::CALL,  n); }
        if (op == "JUMP")         { int v; ss >> v; return Instruction::makeInt(OpCode::JUMP,          v); }
        if (op == "JIF_FALSE")    { int v; ss >> v; return Instruction::makeInt(OpCode::JUMP_IF_FALSE, v); }

        if (op == "POP")    return Instruction::make(OpCode::POP);
        if (op == "ADD")    return Instruction::make(OpCode::ADD);
        if (op == "SUB")    return Instruction::make(OpCode::SUB);
        if (op == "MUL")    return Instruction::make(OpCode::MUL);
        if (op == "DIV")    return Instruction::make(OpCode::DIV);
        if (op == "MOD")    return Instruction::make(OpCode::MOD);
        if (op == "EQ")     return Instruction::make(OpCode::EQ);
        if (op == "NEQ")    return Instruction::make(OpCode::NEQ);
        if (op == "LT")     return Instruction::make(OpCode::LT);
        if (op == "LTE")    return Instruction::make(OpCode::LTE);
        if (op == "GT")     return Instruction::make(OpCode::GT);
        if (op == "GTE")    return Instruction::make(OpCode::GTE);
        if (op == "AND")    return Instruction::make(OpCode::AND);
        if (op == "OR")     return Instruction::make(OpCode::OR);
        if (op == "NOT")    return Instruction::make(OpCode::NOT);
        if (op == "RETURN") return Instruction::make(OpCode::RETURN);
        if (op == "PRINT")  return Instruction::make(OpCode::PRINT);
        if (op == "HALT")   return Instruction::make(OpCode::HALT);

        throw std::runtime_error("Unknown instruction in exe: " + op);
    };

    for (int i = 0; i < nCode; ++i) {
        std::getline(f, line);
        line = trim(line);
        if (!line.empty()) exe.code.push_back(parseIns(line));
    }
    return exe;
}

int main(int argc, char* argv[]) {
    bool debug = false;
    std::string exePath;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--debug" || a == "-d") debug = true;
        else exePath = a;
    }
    if (exePath.empty()) {
        std::cerr << "Usage: executor <file.exe> [--debug]\n";
        return 1;
    }
    try {
        Executable exe = loadExe(exePath);
        VM vm;
        vm.execute(exe, debug);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[Runtime Error] " << e.what() << "\n";
        return 1;
    }
}