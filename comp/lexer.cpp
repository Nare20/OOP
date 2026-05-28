#include "token.h"
#include "utils.h"
#include <vector>
#include <cctype>
#include <unordered_map>

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"if",     TokenType::KW_IF},
    {"else",   TokenType::KW_ELSE},
    {"while",  TokenType::KW_WHILE},
    {"func",   TokenType::KW_FUNC},
    {"return", TokenType::KW_RETURN},
    {"var",    TokenType::KW_VAR},
    {"print",  TokenType::KW_PRINT},
};

class Lexer {
public:
    explicit Lexer(const std::string& src) : src_(src) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (true) {
            skipWS();
            if (pos_ >= src_.size()) {
                tokens.push_back({TokenType::EOF_TOK, "", 0.0, line_});
                break;
            }
            char c = peek();
            Token t;
            if      (std::isdigit(c))             t = readNumber();
            else if (c == '"')                    t = readString();
            else if (std::isalpha(c) || c == '_') t = readIdent();
            else                                  t = readOp();
            tokens.push_back(std::move(t));
        }
        return tokens;
    }

private:
    std::string src_;
    size_t pos_  = 0;
    int    line_ = 1;

    char peek(int off = 0) const {
        size_t i = pos_ + off;
        return i < src_.size() ? src_[i] : '\0';
    }
    char advance() {
        char c = src_[pos_++];
        if (c == '\n') ++line_;
        return c;
    }

    void skipWS() {
        while (pos_ < src_.size()) {
            char c = peek();
            if (std::isspace(c)) { advance(); continue; }
            if (c == '/' && peek(1) == '/') {
                while (pos_ < src_.size() && peek() != '\n') advance();
                continue;
            }
            if (c == '/' && peek(1) == '*') {
                advance(); advance();
                while (pos_ < src_.size()) {
                    if (peek() == '*' && peek(1) == '/') { advance(); advance(); break; }
                    advance();
                }
                continue;
            }
            break;
        }
    }

    Token readNumber() {
        std::string s;
        while (pos_ < src_.size() && (std::isdigit(peek()) || peek() == '.'))
            s += advance();
        return {TokenType::NUMBER, s, std::stod(s), line_};
    }

    Token readString() {
        advance();
        std::string s;
        while (pos_ < src_.size() && peek() != '"') {
            char c = advance();
            if (c == '\\') {
                char e = advance();
                switch (e) {
                    case 'n': s += '\n'; break;
                    case 't': s += '\t'; break;
                    default:  s += e;
                }
            } else s += c;
        }
        if (pos_ >= src_.size()) throw LexError(line_, "Unterminated string");
        advance();
        return {TokenType::STRING_LIT, s, 0.0, line_};
    }

    Token readIdent() {
        std::string s;
        while (pos_ < src_.size() && (std::isalnum(peek()) || peek() == '_'))
            s += advance();
        auto it = KEYWORDS.find(s);
        return {it != KEYWORDS.end() ? it->second : TokenType::IDENTIFIER, s, 0.0, line_};
    }

    Token readOp() {
        int  ln = line_;
        char c  = advance();
        auto two = [&](char nx, TokenType yes, const std::string& yL,
                        TokenType no,  const std::string& nL) -> Token {
            if (peek() == nx) { advance(); return {yes, yL, 0.0, ln}; }
            return {no, nL, 0.0, ln};
        };
        switch (c) {
            case '+': return {TokenType::PLUS,     "+", 0.0, ln};
            case '-': return {TokenType::MINUS,    "-", 0.0, ln};
            case '*': return {TokenType::STAR,     "*", 0.0, ln};
            case '/': return {TokenType::SLASH,    "/", 0.0, ln};
            case '%': return {TokenType::PERCENT,  "%", 0.0, ln};
            case '(': return {TokenType::LPAREN,   "(", 0.0, ln};
            case ')': return {TokenType::RPAREN,   ")", 0.0, ln};
            case '{': return {TokenType::LBRACE,   "{", 0.0, ln};
            case '}': return {TokenType::RBRACE,   "}", 0.0, ln};
            case ';': return {TokenType::SEMICOLON,";", 0.0, ln};
            case ',': return {TokenType::COMMA,    ",", 0.0, ln};
            case '=': return two('=', TokenType::EQ,  "==", TokenType::ASSIGN,"=");
            case '!': return two('=', TokenType::NEQ, "!=", TokenType::NOT,   "!");
            case '<': return two('=', TokenType::LTE, "<=", TokenType::LT,    "<");
            case '>': return two('=', TokenType::GTE, ">=", TokenType::GT,    ">");
            case '&': if (peek()=='&'){advance(); return {TokenType::AND,"&&",0.0,ln};} break;
            case '|': if (peek()=='|'){advance(); return {TokenType::OR, "||",0.0,ln};} break;
            default: break;
        }
        return {TokenType::UNKNOWN, std::string(1,c), 0.0, ln};
    }
};

// Public API — called by mainCompiler
std::vector<Token> runLexer(const std::string& source) {
    return Lexer(source).tokenize();
}