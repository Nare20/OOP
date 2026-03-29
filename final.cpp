#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <variant>
#include <cctype>
#include <stdexcept>

enum class Token {
    num, add, sub, mul, div, mod,
    parent_left, parent_right,
    identifier, assign, semicolon
};

using token_t = std::pair<Token, std::variant<std::monostate, long long, std::string>>;

std::vector<token_t> tokenize(const std::string &input) {
    std::vector<token_t> tokens;
    std::string buf_num;
    std::string buf_id;

    auto flush_number = [&]() {
        if (!buf_num.empty()) {
            tokens.emplace_back(Token::num, std::stoll(buf_num));
            buf_num.clear();
        }
    };

    auto flush_identifier = [&]() {
        if (!buf_id.empty()) {
            tokens.emplace_back(Token::identifier, buf_id);
            buf_id.clear();
        }
    };

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];

        if (std::isspace(c)) continue;

        if (std::isdigit(c)) {
            buf_num += c;
            continue;
        }

        if (std::isalpha(c)) {
            buf_id += c;
            continue;
        }

        flush_number();
        flush_identifier();

        switch (c) {
            case '+': tokens.emplace_back(Token::add, std::monostate{}); break;
            case '-': tokens.emplace_back(Token::sub, std::monostate{}); break;
            case '*': tokens.emplace_back(Token::mul, std::monostate{}); break;
            case '/': tokens.emplace_back(Token::div, std::monostate{}); break;
            case '%': tokens.emplace_back(Token::mod, std::monostate{}); break;
            case '(': tokens.emplace_back(Token::parent_left, std::monostate{}); break;
            case ')': tokens.emplace_back(Token::parent_right, std::monostate{}); break;
            case '=': tokens.emplace_back(Token::assign, std::monostate{}); break;
            case ';': tokens.emplace_back(Token::semicolon, std::monostate{}); break;
            default: throw std::logic_error("Unknown character");
        }
    }

    flush_number();
    flush_identifier();
    return tokens;
}

struct Node {
    std::string type;
    std::variant<long long, std::string, Token> value;
    Node* left = nullptr;
    Node* right = nullptr;
};

Node* parse_expression(std::vector<token_t>& tokens, size_t& pos);

Node* parse_factor(std::vector<token_t>& tokens, size_t& pos) {
    auto [tok, val] = tokens[pos];

    if (tok == Token::num) {
        pos++;
        return new Node{"num", std::get<long long>(val)};
    }

    if (tok == Token::identifier) {
        pos++;
        return new Node{"var", std::get<std::string>(val)};
    }

    if (tok == Token::parent_left) {
        pos++;
        Node* n = parse_expression(tokens, pos);

        if (pos >= tokens.size() || tokens[pos].first != Token::parent_right)
            throw std::logic_error("Missing ')'");

        pos++;
        return n;
    }

    throw std::logic_error("Unexpected token");
}

Node* parse_term(std::vector<token_t>& tokens, size_t& pos) {
    Node* left = parse_factor(tokens, pos);

    while (pos < tokens.size() &&
           (tokens[pos].first == Token::mul ||
            tokens[pos].first == Token::div ||
            tokens[pos].first == Token::mod)) {

        Token op = tokens[pos].first;
        pos++;

        Node* right = parse_factor(tokens, pos);

        Node* n = new Node{"op", op};
        n->left = left;
        n->right = right;

        left = n;
    }

    return left;
}

Node* parse_expression(std::vector<token_t>& tokens, size_t& pos) {
    Node* left = parse_term(tokens, pos);

    while (pos < tokens.size() &&
           (tokens[pos].first == Token::add ||
            tokens[pos].first == Token::sub)) {

        Token op = tokens[pos].first;
        pos++;

        Node* right = parse_term(tokens, pos);

        Node* n = new Node{"op", op};
        n->left = left;
        n->right = right;

        left = n;
    }

    return left;
}

Node* parse_statement(std::vector<token_t>& tokens, size_t& pos) {
    if (pos + 1 < tokens.size() &&
        tokens[pos].first == Token::identifier &&
        tokens[pos + 1].first == Token::assign) {

        std::string var = std::get<std::string>(tokens[pos].second);
        pos += 2;

        Node* expr = parse_expression(tokens, pos);

        Node* n = new Node{"assign", var};
        n->left = expr;

        if (pos < tokens.size() && tokens[pos].first == Token::semicolon)
            pos++;

        return n;
    }

    Node* expr = parse_expression(tokens, pos);

    if (pos < tokens.size() && tokens[pos].first == Token::semicolon)
        pos++;

    return expr;
}

enum class Instruction {
    LOAD, LOAD_VAR, STORE,
    ADD, SUB, MUL, DIV, MOD
};

struct Insn {
    Instruction op;
    int dst;
    std::variant<long long, int, std::string> arg;
};

int reg_counter = 1;

int new_reg() {
    return reg_counter++;
}

void generate(Node* node, std::vector<Insn>& code, int target) {
    if (node->type == "num") {
        code.push_back({Instruction::LOAD, target, std::get<long long>(node->value)});
        return;
    }

    if (node->type == "var") {
        code.push_back({Instruction::LOAD_VAR, target, std::get<std::string>(node->value)});
        return;
    }

    if (node->type == "op") {
        int r1 = new_reg();
        int r2 = new_reg();

        generate(node->left, code, r1);
        generate(node->right, code, r2);

        Token op = std::get<Token>(node->value);

        switch (op) {
            case Token::add: code.push_back({Instruction::ADD, target, r1}); break;
            case Token::sub: code.push_back({Instruction::SUB, target, r1}); break;
            case Token::mul: code.push_back({Instruction::MUL, target, r1}); break;
            case Token::div: code.push_back({Instruction::DIV, target, r1}); break;
            case Token::mod: code.push_back({Instruction::MOD, target, r1}); break;
            default: throw std::logic_error("Invalid operator");
        }

        switch (op) {
            case Token::add: code.push_back({Instruction::ADD, target, r2}); break;
            case Token::sub: code.push_back({Instruction::SUB, target, r2}); break;
            case Token::mul: code.push_back({Instruction::MUL, target, r2}); break;
            case Token::div: code.push_back({Instruction::DIV, target, r2}); break;
            case Token::mod: code.push_back({Instruction::MOD, target, r2}); break;
            default: throw std::logic_error("Invalid operator");
        }

        return;
    }

    if (node->type == "assign") {
        int r = new_reg();
        generate(node->left, code, r);
        code.push_back({Instruction::STORE, r, std::get<std::string>(node->value)});
        code.push_back({Instruction::LOAD_VAR, target, std::get<std::string>(node->value)});
        return;
    }
}

std::map<std::string, long long> symbols;
std::map<int, long long> regs;

void execute(const std::vector<Insn>& code) {
    for (auto &ins : code) {
        switch (ins.op) {
            case Instruction::LOAD:
                regs[ins.dst] = std::get<long long>(ins.arg);
                break;

            case Instruction::LOAD_VAR:
                regs[ins.dst] = symbols[std::get<std::string>(ins.arg)];
                break;

            case Instruction::STORE:
                symbols[std::get<std::string>(ins.arg)] = regs[ins.dst];
                break;

            case Instruction::ADD:
                regs[ins.dst] += regs[std::get<int>(ins.arg)];
                break;

            case Instruction::SUB:
                regs[ins.dst] -= regs[std::get<int>(ins.arg)];
                break;

            case Instruction::MUL:
                regs[ins.dst] *= regs[std::get<int>(ins.arg)];
                break;

            case Instruction::DIV:
                regs[ins.dst] /= regs[std::get<int>(ins.arg)];
                break;

            case Instruction::MOD:
                regs[ins.dst] %= regs[std::get<int>(ins.arg)];
                break;
        }
    }
}

int main() {
    std::string input;
    std::getline(std::cin, input);

    auto tokens = tokenize(input);

    size_t pos = 0;
    std::vector<Node*> statements;

    while (pos < tokens.size())
        statements.push_back(parse_statement(tokens, pos));

    std::vector<Insn> code;

    for (auto stmt : statements)
        generate(stmt, code, 0);

    execute(code);

    std::cout << regs[0] << "\n";
    return 0;
}
