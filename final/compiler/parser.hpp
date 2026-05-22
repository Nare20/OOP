#pragma once
#include <memory>
#include <string>
#include "tokenizer.hpp"
#include "ASTNode.hpp"

class Parser {
public:
    Parser(const std::string &src);
    std::unique_ptr<Program> parse();
private:
    Tokenizer tk;
    Token cur;
    void next();
    bool acceptSymbol(const std::string &s);
    bool acceptKeyword(const std::string &k);
    void expectSymbol(const std::string &s);
    void expectKeyword(const std::string &k);

    std::unique_ptr<FuncNode> parseFunction();
    std::unique_ptr<BlockNode> parseBlock();
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseRelational();
    std::unique_ptr<Expr> parseAddSub();
    std::unique_ptr<Expr> parseMulDiv();
    std::unique_ptr<Expr> parsePrimary();
};
