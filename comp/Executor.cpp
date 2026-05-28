#include "token.h"
#include "ast.h"
#include "instruction.h"
#include "compiler_core.h"
#include "VM.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <vector>


std::vector<Token> runLexer (const std::string& source);
Program            runParser(std::vector<Token> tokens);


bool runSource(const std::string& src, bool debug) {
    try {
        auto tokens  = runLexer(src);
        Program ast  = runParser(std::move(tokens));
        Compiler compiler;
        Executable exe = compiler.compile(ast);
        VM vm;
        vm.execute(exe, debug);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << "\n";
        return false;
    }
}


void runRepl(bool debug) {
    std::cout << "MiniLang REPL  (exit | --debug)\n";
    std::string line, buf;
    int depth = 0;
    while (true) {
        std::cout << (buf.empty() ? ">>> " : "... ");
        if (!std::getline(std::cin, line)) break;
        if (line == "exit" || line == "quit") break;
        if (line == "--debug") { debug = !debug; std::cout << "debug " << (debug?"on":"off") << "\n"; continue; }
        for (char c : line) { if (c=='{') ++depth; if (c=='}') --depth; }
        buf += line + "\n";
        if (depth <= 0) { runSource(buf, debug); buf.clear(); depth = 0; }
    }
    std::cout << "Bye!\n";
}
