#include "parser.hpp"
#include <stdexcept>

Parser::Parser(const std::string &src): tk(src) { next(); }

void Parser::next(){ cur = tk.next(); }

bool Parser::acceptSymbol(const std::string &s){
    if(cur.type == TokenType::Symbol && cur.text == s){ next(); return true; }
    return false;
}

bool Parser::acceptKeyword(const std::string &k){
    if(cur.type == TokenType::Keyword && cur.text == k){ next(); return true; }
    return false;
}

void Parser::expectSymbol(const std::string &s){ if(!acceptSymbol(s)) throw std::runtime_error("expected '" + s + "'"); }
void Parser::expectKeyword(const std::string &k){ if(!acceptKeyword(k)) throw std::runtime_error("expected keyword '" + k + "'"); }

std::unique_ptr<Program> Parser::parse(){
    auto program = std::make_unique<Program>();
    while(cur.type != TokenType::End){
        if(cur.type == TokenType::Keyword && cur.text == "func"){
            program->funcs.push_back(parseFunction());
        } else {
            throw std::runtime_error("expected function declaration");
        }
    }
    return program;
}

std::unique_ptr<FuncNode> Parser::parseFunction(){
    expectKeyword("func");
    if(cur.type != TokenType::Ident) throw std::runtime_error("expected function name");
    std::string name = cur.text; next();
    expectSymbol("(");
    std::vector<std::string> params;
    if(!acceptSymbol(")")){
        while(true){
            if(cur.type != TokenType::Ident) throw std::runtime_error("expected parameter name");
            params.push_back(cur.text);
            next();
            if(acceptSymbol(",")) continue;
            break;
        }
        expectSymbol(")");
    }
    auto body = parseBlock();
    auto fn = std::make_unique<FuncNode>();
    fn->name = name;
    fn->params = std::move(params);
    fn->body = std::move(body);
    return fn;
}

std::unique_ptr<BlockNode> Parser::parseBlock(){
    expectSymbol("{");
    auto block = std::make_unique<BlockNode>();
    while(!(cur.type == TokenType::Symbol && cur.text == "}")){
        block->stmts.push_back(parseStatement());
    }
    expectSymbol("}");
    return block;
}

std::unique_ptr<Stmt> Parser::parseStatement(){
    if(acceptKeyword("return")){
        auto expr = parseExpression();
        expectSymbol(";");
        return std::make_unique<ReturnNode>(std::move(expr));
    }
    if(acceptKeyword("if")){
        expectSymbol("(");
        auto cond = parseExpression();
        expectSymbol(")");
        auto thenBlock = parseBlock();
        std::unique_ptr<BlockNode> elseBlock;
        if(acceptKeyword("else")) elseBlock = parseBlock();
        auto node = std::make_unique<IfNode>();
        node->cond = std::move(cond);
        node->thenBlock = std::move(thenBlock);
        node->elseBlock = std::move(elseBlock);
        return node;
    }
    if(acceptKeyword("while")){
        expectSymbol("(");
        auto cond = parseExpression();
        expectSymbol(")");
        auto body = parseBlock();
        auto node = std::make_unique<WhileNode>();
        node->cond = std::move(cond);
        node->body = std::move(body);
        return node;
    }
    if(cur.type == TokenType::Symbol && cur.text == "{"){
        return parseBlock();
    }
    if(cur.type == TokenType::Ident){
        std::string name = cur.text;
        next();
        if(acceptSymbol("=")){
            auto rhs = parseExpression();
            expectSymbol(";");
            return std::make_unique<AssignNode>(name, std::move(rhs));
        }
        if(acceptSymbol("(")){
            auto call = std::make_unique<CallExpr>(name);
            if(!acceptSymbol(")")){
                while(true){ call->args.push_back(parseExpression()); if(acceptSymbol(",")) continue; break; }
                expectSymbol(")");
            }
            expectSymbol(";");
            return std::make_unique<ExprStmt>(std::move(call));
        }
        throw std::runtime_error("unexpected identifier in statement");
    }
    auto expr = parseExpression();
    expectSymbol(";");
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<Expr> Parser::parseExpression(){
    return parseEquality();
}

std::unique_ptr<Expr> Parser::parseEquality(){
    auto left = parseRelational();
    while(cur.type == TokenType::Symbol && (cur.text == "==" || cur.text == "!=")){
        std::string op = cur.text;
        next();
        auto right = parseRelational();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::parseRelational(){
    auto left = parseAddSub();
    while(cur.type == TokenType::Symbol && (cur.text == "<" || cur.text == "<=" || cur.text == ">" || cur.text == ">=")){
        std::string op = cur.text;
        next();
        auto right = parseAddSub();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::parseAddSub(){
    auto left = parseMulDiv();
    while(cur.type == TokenType::Symbol && (cur.text == "+" || cur.text == "-")){
        std::string op = cur.text;
        next();
        auto right = parseMulDiv();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::parseMulDiv(){
    auto left = parsePrimary();
    while(cur.type == TokenType::Symbol && (cur.text == "*" || cur.text == "/")){
        std::string op = cur.text;
        next();
        auto right = parsePrimary();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::parsePrimary(){
    if(cur.type == TokenType::Number){
        int value = cur.value;
        next();
        return std::make_unique<NumberExpr>(value);
    }
    if(cur.type == TokenType::Ident || (cur.type == TokenType::Keyword && cur.text == "print")){
        std::string name = cur.text;
        next();
        if(acceptSymbol("(")){
            auto call = std::make_unique<CallExpr>(name);
            if(!acceptSymbol(")")){
                while(true){
                    call->args.push_back(parseExpression());
                    if(acceptSymbol(",")) continue;
                    break;
                }
                expectSymbol(")");
            }
            return call;
        }
        return std::make_unique<VarExpr>(name);
    }
    if(acceptSymbol("(")){
        auto expr = parseExpression();
        expectSymbol(")");
        return expr;
    }
    throw std::runtime_error("unexpected token in expression");
}
