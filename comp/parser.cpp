#include "token.h"
#include "ast.h"
#include "utils.h"
#include <vector>
#include <sstream>

class Parser {
public:
    explicit Parser(std::vector<Token> toks) : toks_(std::move(toks)) {}

    Program parse() {
        Program prog;
        while (!check(TokenType::EOF_TOK))
            prog.stmts.push_back(parseStmt());
        return prog;
    }

private:
    std::vector<Token> toks_;
    size_t pos_ = 0;

    // ── Helpers ──────────────────────────────────────────────────────────────
    const Token& peek(int off = 0) const {
        size_t i = pos_ + off;
        return i < toks_.size() ? toks_[i] : toks_.back();
    }
    const Token& advance() {
        const Token& t = toks_[pos_];
        if (pos_ + 1 < toks_.size()) ++pos_;
        return t;
    }
    bool check(TokenType t) const { return peek().type == t; }
    bool match(TokenType t) { if (check(t)){ advance(); return true; } return false; }
    const Token& expect(TokenType t, const std::string& msg) {
        if (!check(t)) throw ParseError(peek().line, msg + " (got '" + peek().lexeme + "')");
        return advance();
    }

    // ── AST builders ─────────────────────────────────────────────────────────
    static ExprPtr mkNum(double v) {
        auto e = std::make_unique<Expr>(); e->kind = Expr::Kind::Number; e->number={v}; return e;
    }
    static ExprPtr mkStr(const std::string& s) {
        auto e = std::make_unique<Expr>(); e->kind = Expr::Kind::String; e->string={s}; return e;
    }
    static ExprPtr mkVar(const std::string& n) {
        auto e = std::make_unique<Expr>(); e->kind = Expr::Kind::Var; e->var={n}; return e;
    }
    static ExprPtr mkBin(const std::string& op, ExprPtr l, ExprPtr r) {
        auto e = std::make_unique<Expr>(); e->kind = Expr::Kind::Binary;
        e->binary={op, std::move(l), std::move(r)}; return e;
    }
    static ExprPtr mkUn(const std::string& op, ExprPtr operand) {
        auto e = std::make_unique<Expr>(); e->kind = Expr::Kind::Unary;
        e->unary={op, std::move(operand)}; return e;
    }
    static ExprPtr mkAssign(const std::string& n, ExprPtr val) {
        auto e = std::make_unique<Expr>(); e->kind = Expr::Kind::Assign;
        e->assign={n, std::move(val)}; return e;
    }
    static ExprPtr mkCall(const std::string& c, std::vector<ExprPtr> args) {
        auto e = std::make_unique<Expr>(); e->kind = Expr::Kind::Call;
        e->call={c, std::move(args)}; return e;
    }

    // ── Statements ────────────────────────────────────────────────────────────
    StmtPtr parseStmt() {
        if (check(TokenType::KW_FUNC))   return parseFuncDecl();
        if (check(TokenType::KW_VAR))    return parseVarDecl();
        if (check(TokenType::KW_IF))     return parseIf();
        if (check(TokenType::KW_WHILE))  return parseWhile();
        if (check(TokenType::KW_RETURN)) return parseReturn();
        if (check(TokenType::KW_PRINT))  return parsePrint();
        return parseExprStmt();
    }

    StmtPtr parseFuncDecl() {
        expect(TokenType::KW_FUNC, "Expected 'func'");
        std::string name = expect(TokenType::IDENTIFIER, "Expected function name").lexeme;
        expect(TokenType::LPAREN, "Expected '('");
        std::vector<std::string> params;
        if (!check(TokenType::RPAREN)) {
            do { params.push_back(expect(TokenType::IDENTIFIER,"Expected param").lexeme); }
            while (match(TokenType::COMMA));
        }
        expect(TokenType::RPAREN, "Expected ')'");
        auto body = parseBlock();
        auto s = std::make_unique<Stmt>();
        s->kind = Stmt::Kind::FuncDecl;
        s->funcDecl = {name, std::move(params), std::move(body)};
        return s;
    }

    StmtPtr parseVarDecl() {
        expect(TokenType::KW_VAR, "Expected 'var'");
        std::string name = expect(TokenType::IDENTIFIER, "Expected variable name").lexeme;
        ExprPtr init = match(TokenType::ASSIGN) ? parseExpr() : mkNum(0.0);
        expect(TokenType::SEMICOLON, "Expected ';'");
        auto s = std::make_unique<Stmt>();
        s->kind = Stmt::Kind::VarDecl;
        s->varDecl = {name, std::move(init)};
        return s;
    }

    StmtPtr parseIf() {
        expect(TokenType::KW_IF, "Expected 'if'");
        expect(TokenType::LPAREN, "Expected '('");
        auto cond = parseExpr();
        expect(TokenType::RPAREN, "Expected ')'");
        auto thenB = parseBlock();
        std::unique_ptr<BlockStmt> elseB;
        if (match(TokenType::KW_ELSE)) elseB = parseBlock();
        auto s = std::make_unique<Stmt>();
        s->kind = Stmt::Kind::If;
        s->ifStmt = {std::move(cond), std::move(thenB), std::move(elseB)};
        return s;
    }

    StmtPtr parseWhile() {
        expect(TokenType::KW_WHILE, "Expected 'while'");
        expect(TokenType::LPAREN, "Expected '('");
        auto cond = parseExpr();
        expect(TokenType::RPAREN, "Expected ')'");
        auto body = parseBlock();
        auto s = std::make_unique<Stmt>();
        s->kind = Stmt::Kind::While;
        s->whileStmt = {std::move(cond), std::move(body)};
        return s;
    }

    StmtPtr parseReturn() {
        expect(TokenType::KW_RETURN, "Expected 'return'");
        auto val = parseExpr();
        expect(TokenType::SEMICOLON, "Expected ';'");
        auto s = std::make_unique<Stmt>(); s->kind = Stmt::Kind::Return;
        s->ret = {std::move(val)}; return s;
    }

    StmtPtr parsePrint() {
        expect(TokenType::KW_PRINT, "Expected 'print'");
        expect(TokenType::LPAREN, "Expected '('");
        auto val = parseExpr();
        expect(TokenType::RPAREN, "Expected ')'");
        expect(TokenType::SEMICOLON, "Expected ';'");
        auto s = std::make_unique<Stmt>(); s->kind = Stmt::Kind::Print;
        s->print = {std::move(val)}; return s;
    }

    StmtPtr parseExprStmt() {
        auto e = parseExpr();
        expect(TokenType::SEMICOLON, "Expected ';'");
        auto s = std::make_unique<Stmt>(); s->kind = Stmt::Kind::Expr;
        s->expr = {std::move(e)}; return s;
    }

    std::unique_ptr<BlockStmt> parseBlock() {
        expect(TokenType::LBRACE, "Expected '{'");
        auto b = std::make_unique<BlockStmt>();
        while (!check(TokenType::RBRACE) && !check(TokenType::EOF_TOK))
            b->stmts.push_back(parseStmt());
        expect(TokenType::RBRACE, "Expected '}'");
        return b;
    }

    // ── Expressions (recursive descent, precedence climbing) ─────────────────
    ExprPtr parseExpr() { return parseAssign(); }

    ExprPtr parseAssign() {
        if (check(TokenType::IDENTIFIER) && pos_+1 < toks_.size() &&
            toks_[pos_+1].type == TokenType::ASSIGN) {
            std::string name = advance().lexeme; advance();
            return mkAssign(name, parseAssign());
        }
        return parseOr();
    }
    ExprPtr parseOr() {
        auto l = parseAnd();
        while (check(TokenType::OR)) { advance(); l = mkBin("||", std::move(l), parseAnd()); }
        return l;
    }
    ExprPtr parseAnd() {
        auto l = parseEq();
        while (check(TokenType::AND)) { advance(); l = mkBin("&&", std::move(l), parseEq()); }
        return l;
    }
    ExprPtr parseEq() {
        auto l = parseCmp();
        while (check(TokenType::EQ) || check(TokenType::NEQ)) {
            std::string op = advance().lexeme; l = mkBin(op, std::move(l), parseCmp());
        }
        return l;
    }
    ExprPtr parseCmp() {
        auto l = parseAdd();
        while (check(TokenType::LT)||check(TokenType::LTE)||
               check(TokenType::GT)||check(TokenType::GTE)) {
            std::string op = advance().lexeme; l = mkBin(op, std::move(l), parseAdd());
        }
        return l;
    }
    ExprPtr parseAdd() {
        auto l = parseMul();
        while (check(TokenType::PLUS)||check(TokenType::MINUS)) {
            std::string op = advance().lexeme; l = mkBin(op, std::move(l), parseMul());
        }
        return l;
    }
    ExprPtr parseMul() {
        auto l = parseUnary();
        while (check(TokenType::STAR)||check(TokenType::SLASH)||check(TokenType::PERCENT)) {
            std::string op = advance().lexeme; l = mkBin(op, std::move(l), parseUnary());
        }
        return l;
    }
    ExprPtr parseUnary() {
        if (check(TokenType::NOT)||check(TokenType::MINUS)) {
            std::string op = advance().lexeme;
            return mkUn(op, parseUnary());
        }
        return parsePrimary();
    }
    ExprPtr parsePrimary() {
        if (check(TokenType::NUMBER))     { double v = peek().numVal; advance(); return mkNum(v); }
        if (check(TokenType::STRING_LIT)) { std::string s = peek().lexeme; advance(); return mkStr(s); }
        if (check(TokenType::IDENTIFIER)) {
            std::string name = advance().lexeme;
            if (match(TokenType::LPAREN)) {
                std::vector<ExprPtr> args;
                if (!check(TokenType::RPAREN)) {
                    do { args.push_back(parseExpr()); } while (match(TokenType::COMMA));
                }
                expect(TokenType::RPAREN, "Expected ')'");
                return mkCall(name, std::move(args));
            }
            return mkVar(name);
        }
        if (match(TokenType::LPAREN)) {
            auto e = parseExpr();
            expect(TokenType::RPAREN, "Expected ')'");
            return e;
        }
        throw ParseError(peek().line, "Unexpected token '" + peek().lexeme + "'");
    }
};

// Public API — called by mainCompiler
Program runParser(std::vector<Token> tokens) {
    return Parser(std::move(tokens)).parse();
}