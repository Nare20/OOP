#include <fstream>
#include <sstream>
#include <iostream>
#include "parser.hpp"
#include "IRGenerator.hpp"
#include "BytecodeGenerator.hpp"

int main(int argc, char **argv){
    if(argc < 2){ std::cerr << "usage: compiler file.src [-o out.bc]" << std::endl; return 1; }
    std::string inPath = argv[1];
    std::string outPath = "out.bc";
    for(int i = 2; i < argc; ++i){
        if(std::string(argv[i]) == "-o" && i + 1 < argc){ outPath = argv[++i]; }
    }
    std::ifstream in(inPath);
    if(!in){ std::cerr << "cannot open input" << std::endl; return 1; }
    std::ostringstream ss; ss << in.rdbuf();
    std::string src = ss.str();
    try{
        Parser parser(src);
        auto program = parser.parse();
        IRGenerator irGen;
        auto ir = irGen.generate(*program);
        BytecodeGenerator bcGen;
        auto bytecode = bcGen.generate(ir);
        std::ofstream out(outPath, std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
        std::cout << "Wrote bytecode file: " << outPath << " (" << bytecode.size() << " bytes)" << std::endl;
        return 0;
    }catch(const std::exception &e){
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
}
