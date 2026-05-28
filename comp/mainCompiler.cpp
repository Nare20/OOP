#include "token.h"
#include "ast.h"
#include "instruction.h"
#include "compiler_core.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>

// Forward declarations (defined in lexer.cpp and parser.cpp)
std::vector<Token> runLexer (const std::string& source);
Program            runParser(std::vector<Token> tokens);

// Serialize Executable to a simple text format
static void saveExe(const Executable& exe, const std::string& path) {
    std::ofstream f(path);
    if (!f) throw std::runtime_error("Cannot write: " + path);

    // Functions section
    f << "FUNCTIONS " << exe.functions.size() << "\n";
    for (auto& [name, addr] : exe.functions)
        f << name << " " << addr << "\n";

    // Code section
    f << "CODE " << exe.code.size() << "\n";
    for (auto& ins : exe.code)
        f << ins.toString() << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: compiler <source.ml> [output.exe]\n";
        return 1;
    }
    std::string srcPath = argv[1];
    std::string exePath = (argc >= 3) ? argv[2] : "out.exe";

    try {
        std::string source = readFile(srcPath);

        // 1. Lex
        auto tokens = runLexer(source);

        // 2. Parse
        Program ast = runParser(std::move(tokens));

        // 3. Compile
        Compiler compiler;
        Executable exe = compiler.compile(ast);

        // 4. Save
        saveExe(exe, exePath);
        std::cout << "Compiled: " << srcPath << " -> " << exePath << "\n";
        std::cout << "Instructions: " << exe.code.size() << "\n";
        std::cout << "Functions:    " << exe.functions.size() << "\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Compiler Error] " << e.what() << "\n";
        return 1;
    }
}