#include "token.h"

std::string Token::typeName() const {
    switch (type) {
        case TokenType::NUMBER:     return "NUMBER";
        case TokenType::STRING_LIT: return "STRING";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::KW_IF:      return "if";
        case TokenType::KW_ELSE:    return "else";
        case TokenType::KW_WHILE:   return "while";
        case TokenType::KW_FUNC:    return "func";
        case TokenType::KW_RETURN:  return "return";
        case TokenType::KW_VAR:     return "var";
        case TokenType::KW_PRINT:   return "print";
        case TokenType::PLUS:       return "+";
        case TokenType::MINUS:      return "-";
        case TokenType::STAR:       return "*";
        case TokenType::SLASH:      return "/";
        case TokenType::PERCENT:    return "%";
        case TokenType::EQ:         return "==";
        case TokenType::NEQ:        return "!=";
        case TokenType::LT:         return "<";
        case TokenType::LTE:        return "<=";
        case TokenType::GT:         return ">";
        case TokenType::GTE:        return ">=";
        case TokenType::ASSIGN:     return "=";
        case TokenType::AND:        return "&&";
        case TokenType::OR:         return "||";
        case TokenType::NOT:        return "!";
        case TokenType::LPAREN:     return "(";
        case TokenType::RPAREN:     return ")";
        case TokenType::LBRACE:     return "{";
        case TokenType::RBRACE:     return "}";
        case TokenType::SEMICOLON:  return ";";
        case TokenType::COMMA:      return ",";
        case TokenType::EOF_TOK:    return "EOF";
        default:                    return "UNKNOWN";
    }
}