#pragma once
#include <string>
#include <vector>
#include <stdexcept>

std::string              readFile(const std::string& path);
std::string              trim    (const std::string& s);
std::vector<std::string> split   (const std::string& s, char delim);
std::string              fmtError(int line, const std::string& msg);

class LangError : public std::runtime_error {
public:
    LangError(int line, const std::string& msg)
        : std::runtime_error(fmtError(line, msg)), line_(line) {}
    int line() const { return line_; }
private:
    int line_;
};

class LexError    : public LangError { public: using LangError::LangError; };
class ParseError  : public LangError { public: using LangError::LangError; };
class CompileError: public LangError { public: using LangError::LangError; };
class RuntimeError: public LangError { public: using LangError::LangError; };