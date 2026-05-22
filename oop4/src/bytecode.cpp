#include "bytecode.hpp"
#include <fstream>
#include <iterator>
#include <stdexcept>

std::vector<uint8_t> readBytecodeFile(const std::string &path) {
    std::ifstream in(path, std::ios::binary);
    if(!in) throw std::runtime_error("cannot open bytecode file");
    std::vector<uint8_t> buf((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return buf;
}
