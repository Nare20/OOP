#pragma once
#include <string>

enum class TokenType {
    NUMBER, STRING_LIT, IDENTIFIER,
    KW_IF, KW_ELSE, KW_WHILE, KW_FUNC, KW_RETURN, KW_VAR, KW_PRINT,
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQ, NEQ, LT, LTE, GT, GTE,
    ASSIGN,
    AND, OR, NOT,
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,
    EOF_TOK, UNKNOWN
};

struct Token {
    TokenType   type;
    std::string lexeme;
    double      numVal = 0.0;
    int         line   = 0;
    std::string typeName() const;
};