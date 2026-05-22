#pragma once
#include <string>
#include <vector>

enum class TokenType { End, Number, Ident, Keyword, Symbol };
struct Token { TokenType type; std::string text; int value; int line=1; };

class Tokenizer {
public:
    Tokenizer(const std::string &src);
    Token next();
    Token peek();
    bool eof();
private:
    std::string s; size_t i=0; Token cur;
    void skipWhitespace();
    Token readNumber();
    Token readIdent();
};
