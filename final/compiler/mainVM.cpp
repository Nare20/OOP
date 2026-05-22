#include <iostream>
#include "VM.hpp"
#include "bytecode.hpp"

int main(int argc, char **argv){
    if(argc<2){ std::cerr<<"usage: vm file.bc\n"; return 1; }
    try{
        auto bytecode = readBytecodeFile(argv[1]);
        VM vm;
        vm.load(bytecode);
        return vm.run();
    }catch(const std::exception &e){ std::cerr<<e.what()<<std::endl; return 1; }
}
