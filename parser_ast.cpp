#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>

using namespace std;

enum class TokenType {
    NUM, IDENT,
    PLUS, MINUS, MUL, DIV,
    ASSIGN,
    LPAREN, RPAREN,
    SEMI,
    END
};

struct Token {
    TokenType type;
    string text;
};

vector<Token> tokenize(const string& input) {
    vector<Token> tokens;
    int i = 0;

    while (i < (int)input.size()) {
        char c = input[i];

        if (isspace(c)) { i++; continue; }

        if (isdigit(c)) {
            string num;
            while (i < (int)input.size() && isdigit(input[i])) {
                num.push_back(input[i]);
                i++;
            }
            tokens.push_back({TokenType::NUM, num});
            continue;
        }

        if (isalpha(c)) {
            string id;
            while (i < (int)input.size() && (isalnum(input[i]) || input[i]=='_')) {
                id.push_back(input[i]);
                i++;
            }
            tokens.push_back({TokenType::IDENT, id});
            continue;
        }

        if (c == '+') tokens.push_back({TokenType::PLUS, "+"});
        else if (c == '-') tokens.push_back({TokenType::MINUS, "-"});
        else if (c == '*') tokens.push_back({TokenType::MUL, "*"});
        else if (c == '/') tokens.push_back({TokenType::DIV, "/"});
        else if (c == '=') tokens.push_back({TokenType::ASSIGN, "="});
        else if (c == '(') tokens.push_back({TokenType::LPAREN, "("});
        else if (c == ')') tokens.push_back({TokenType::RPAREN, ")"});
        else if (c == ';') tokens.push_back({TokenType::SEMI, ";"});
        else throw runtime_error("Unknown character!");

        i++;
    }

    tokens.push_back({TokenType::END, "END"});
    return tokens;
}


enum class NodeType {
    NUM,
    VAR,
    ADD, SUB, MUL, DIV,
    ASSIGN,
    PROGRAM
};

struct Node {
    NodeType type;
    int value;
    string name;
    Node* left;
    Node* right;
    vector<Node*> children;

    Node(NodeType t) : type(t), value(0), left(nullptr), right(nullptr) {}
};


class Parser {
private:
    vector<Token> tokens;
    int pos = 0;

    Token current() { return tokens[pos]; }
    void advance() { pos++; }

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

        throw runtime_error("Invalid factor");
    }

    Node* parseTerm() {
        Node* node = parseFactor();

        while (current().type == TokenType::MUL || current().type == TokenType::DIV) {
            TokenType op = current().type;
            advance();

            Node* right = parseFactor();
            Node* parent = new Node(op == TokenType::MUL ? NodeType::MUL : NodeType::DIV);
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
            Node* parent = new Node(op == TokenType::PLUS ? NodeType::ADD : NodeType::SUB);
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

                Node* assign = new Node(NodeType::ASSIGN);
                assign->name = id.text;
                assign->left = expr;

                if (current().type == TokenType::SEMI)
                    advance();

                return assign;
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

void printTree(Node* node, int depth = 0) {
    if (!node) return;

    if (node->type == NodeType::PROGRAM) {
        cout << "PROGRAM\n";
        for (auto child : node->children) {
            printTree(child, depth + 1);
        }
        return;
    }

    printTree(node->right, depth + 1);

    for (int i = 0; i < depth; i++) cout << "    ";

    if (node->type == NodeType::NUM) cout << node->value << "\n";
    else if (node->type == NodeType::VAR) cout << node->name << "\n";
    else if (node->type == NodeType::ADD) cout << "+\n";
    else if (node->type == NodeType::SUB) cout << "-\n";
    else if (node->type == NodeType::MUL) cout << "*\n";
    else if (node->type == NodeType::DIV) cout << "/\n";
    else if (node->type == NodeType::ASSIGN) cout << "ASSIGN(" << node->name << ")\n";
}

int main() {
    string input;
    cout << "Input: ";
    getline(cin, input);

    auto tokens = tokenize(input);
    Parser parser(tokens);

    Node* root = parser.parseProgram();

    cout << "\nAST Tree:\n";
    printTree(root);
}