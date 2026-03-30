#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
#include "Node.hpp"

using namespace std;

// TOKENS 
enum class TokenType {
    NUM,
    IDENT,

    PLUS,
    MINUS,
    MUL,
    DIV,

    ASSIGN,
    SEMI,

    LPAREN,
    RPAREN,

    END
};

struct Token {
    TokenType type;
    string text;

    Token(TokenType t, string s = "") : type(t), text(s) {}
};

// -LEXER 
vector<Token> tokenize(const string& input) {
    vector<Token> tokens;
    int i = 0;

    while (i < (int)input.size()) {
        char c = input[i];

        if (isspace(c)) {
            i++;
            continue;
        }

        if (isdigit(c)) {
            string num;
            while (i < (int)input.size() && isdigit(input[i])) {
                num.push_back(input[i]);
                i++;
            }
            tokens.push_back(Token(TokenType::NUM, num));
            continue;
        }

        if (isalpha(c)) {
            string id;
            while (i < (int)input.size() && (isalnum(input[i]) || input[i] == '_')) {
                id.push_back(input[i]);
                i++;
            }
            tokens.push_back(Token(TokenType::IDENT, id));
            continue;
        }

        if (c == '+') tokens.push_back(Token(TokenType::PLUS, "+"));
        else if (c == '-') tokens.push_back(Token(TokenType::MINUS, "-"));
        else if (c == '*') tokens.push_back(Token(TokenType::MUL, "*"));
        else if (c == '/') tokens.push_back(Token(TokenType::DIV, "/"));
        else if (c == '=') tokens.push_back(Token(TokenType::ASSIGN, "="));
        else if (c == ';') tokens.push_back(Token(TokenType::SEMI, ";"));
        else if (c == '(') tokens.push_back(Token(TokenType::LPAREN, "("));
        else if (c == ')') tokens.push_back(Token(TokenType::RPAREN, ")"));
        else throw runtime_error("Unknown character!");

        i++;
    }

    tokens.push_back(Token(TokenType::END, "END"));
    return tokens;
}

// PARSER
class Parser {
private:
    vector<Token> tokens;
    int pos = 0;

    Token current() {
        return tokens[pos];
    }

    void advance() {
        pos++;
    }

    void expect(TokenType t) {
        if (current().type != t)
            throw runtime_error("Unexpected token!");
    }

    Node* parseFactor() {
        if (current().type == TokenType::NUM) {
            Node* node = new Node(NodeType::NUM);
            node->value = stoi(current().text);
            advance();
            return node;
        }

        if (current().type == TokenType::IDENT) {
            Node* node = new Node(NodeType::VAR);
            node->name = current().text;
            advance();
            return node;
        }

        if (current().type == TokenType::LPAREN) {
            advance();
            Node* node = parseExpression();
            expect(TokenType::RPAREN);
            advance();
            return node;
        }

        throw runtime_error("Invalid factor!");
    }

    Node* parseTerm() {
        Node* node = parseFactor();

        while (current().type == TokenType::MUL || current().type == TokenType::DIV) {
            TokenType op = current().type;
            advance();

            Node* right = parseFactor();

            Node* parent = nullptr;
            if (op == TokenType::MUL) parent = new Node(NodeType::MUL);
            else parent = new Node(NodeType::DIV);

            parent->left = node;
            parent->right = right;
            node = parent;
        }

        return node;
    }

    Node* parseExpression() {
        Node* node = parseTerm();

        while (current().type == TokenType::PLUS || current().type == TokenType::MINUS) {
            TokenType op = current().type;
            advance();

            Node* right = parseTerm();

            Node* parent = nullptr;
            if (op == TokenType::PLUS) parent = new Node(NodeType::ADD);
            else parent = new Node(NodeType::SUB);

            parent->left = node;
            parent->right = right;
            node = parent;
        }

        return node;
    }

    Node* parseStatement() {
        if (current().type == TokenType::IDENT) {
            Token id = current();
            advance();

            if (current().type == TokenType::ASSIGN) {
                advance();
                Node* expr = parseExpression();

                Node* assignNode = new Node(NodeType::ASSIGN);
                assignNode->name = id.text;
                assignNode->left = expr;

                if (current().type == TokenType::SEMI)
                    advance();

                return assignNode;
            }

            pos--; 
        }

        Node* expr = parseExpression();
        if (current().type == TokenType::SEMI)
            advance();

        return expr;
    }

public:
    Parser(const vector<Token>& t) : tokens(t) {}

    Node* parseProgram() {
        Node* prog = new Node(NodeType::PROGRAM);

        while (current().type != TokenType::END) {
            prog->children.push_back(parseStatement());
        }

        return prog;
    }
};

Node* parse(const string& input) {
    vector<Token> tokens = tokenize(input);
    Parser parser(tokens);
    return parser.parseProgram();
}