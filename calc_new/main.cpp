#include <iostream>
#include "Compiler.hpp"
#include "VM.hpp"

int main() {
    std::string input;

    std::cout << "Enter program (example: a=5; b=2; a+6*b)\n";
    std::getline(std::cin, input);

    Compiler compiler;
    compiler.compile(input);

    VM vm((int)compiler.sym.table.size());
    vm.execute(compiler.code);

    std::cout << "\nResult in R0 = " << vm.R[0] << "\n";

    std::cout << "\nVariables:\n";
    for (auto& [name, idx] : compiler.sym.table) {
        std::cout << name << " = " << vm.data[idx] << "\n";
    }

    return 0;
}
