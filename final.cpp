#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <variant>
#include <cctype>
#include <stdexcept>
#include <cmath>

//  Tokenizer ,Lexer
enum class Token {
    num, add, sub, mul, div, mod, pow,
    parent_left, parent_right,
    identifier, assign, semicolon
};

using token_t = std::pair<Token, std::variant<std::monostate, long long, std::string>>;

std::vector<token_t> tokenize(const std::string &input) {
    std::vector<token_t> tokens;
    std::string buf;

    auto flush_number = [&]() {
        if (!buf.empty()) {
            tokens.emplace_back(Token::num, std::stoll(buf));
            buf.clear();
        }
    };
    auto flush_identifier = [&]() {
        if (!buf.empty()) {
            tokens.emplace_back(Token::identifier, buf);
            buf.clear();
        }
    };

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (std::isspace(c)) continue;

        if (std::isdigit(c)) { buf += c; continue; }
        if (std::isalpha(c)) { buf += c; continue; }

        flush_number();
        flush_identifier();

        switch (c) {
            case '+': tokens.emplace_back(Token::add, std::monostate{}); break;
            case '-': tokens.emplace_back(Token::sub, std::monostate{}); break;
            case '*': tokens.emplace_back(Token::mul, std::monostate{}); break;
            case '/': tokens.emplace_back(Token::div, std::monostate{}); break;
            case '%': tokens.emplace_back(Token::mod, std::monostate{}); break;
            case '^': tokens.emplace_back(Token::pow, std::monostate{}); break;
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

//AST
struct Node {
    std::string type; 
    std::variant<long long, std::string, Token> value;
    Node* left = nullptr;
    Node* right = nullptr;
};

//parser
Node* parse_expression(std::vector<token_t>& tokens, size_t& pos);

Node* parse_factor(std::vector<token_t>& tokens, size_t& pos) {
    auto [tok, val] = tokens[pos];
    if (tok == Token::num) { pos++; return new Node{"num", std::get<long long>(val)}; }
    if (tok == Token::identifier) { pos++; return new Node{"var", std::get<std::string>(val)}; }
    if (tok == Token::parent_left) {
        pos++;
        Node* n = parse_expression(tokens, pos);
        if (tokens[pos].first != Token::parent_right)
            throw std::logic_error("Missing ')'");
        pos++;
        return n;
    }
    throw std::logic_error("Unexpected token in factor");
}

Node* parse_term(std::vector<token_t>& tokens, size_t& pos) {
    Node* left = parse_factor(tokens, pos);
    while (pos < tokens.size() && (tokens[pos].first == Token::mul || tokens[pos].first == Token::div || tokens[pos].first == Token::mod)) {
        Token op = tokens[pos].first; pos++;
        Node* right = parse_factor(tokens, pos);
        Node* n = new Node{"op", op}; n->left = left; n->right = right;
        left = n;
    }
    return left;
}

Node* parse_expression(std::vector<token_t>& tokens, size_t& pos) {
    Node* left = parse_term(tokens, pos);
    while (pos < tokens.size() && (tokens[pos].first == Token::add || tokens[pos].first == Token::sub)) {
        Token op = tokens[pos].first; pos++;
        Node* right = parse_term(tokens, pos);
        Node* n = new Node{"op", op}; n->left = left; n->right = right;
        left = n;
    }
    return left;
}

//  parsing
Node* parse_statement(std::vector<token_t>& tokens, size_t& pos) {
    if (tokens[pos].first == Token::identifier && tokens[pos+1].first == Token::assign) {
        std::string var = std::get<std::string>(tokens[pos].second); pos += 2;
        Node* expr = parse_expression(tokens, pos);
        Node* n = new Node{"assign", var}; n->left = expr;
        if (pos < tokens.size() && tokens[pos].first == Token::semicolon) pos++;
        return n;
    } else {
        Node* expr = parse_expression(tokens, pos);
        if (pos < tokens.size() && tokens[pos].first == Token::semicolon) pos++;
        return expr;
    }
}

// Code Generator 
enum class Instruction { LOAD, LOAD_VAR, STORE, ADD, SUB, MUL, DIV, MOD };

struct Insn {
    Instruction op;
    int reg;
    std::variant<int,long long,std::string> operand;
};

int reg_counter = 0; 
int new_reg() { return reg_counter++; }

void generate(Node* node, std::vector<Insn>& code, int target, std::map<std::string,int>& var_map) {
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
        generate(node->left, code, r1, var_map);
        generate(node->right, code, r2, var_map);
        Token op = std::get<Token>(node->value);
        Instruction instr;
        switch(op) {
            case Token::add: instr = Instruction::ADD; break;
            case Token::sub: instr = Instruction::SUB; break;
            case Token::mul: instr = Instruction::MUL; break;
            case Token::div: instr = Instruction::DIV; break;
            case Token::mod: instr = Instruction::MOD; break;
            default: throw std::logic_error("Unknown op");
        }
        code.push_back({instr, target, r1}); 
        code.push_back({instr, target, r2}); 
        return;
    }
    if (node->type == "assign") {
        int r = new_reg();
        generate(node->left, code, r, var_map);
        code.push_back({Instruction::STORE, r, std::get<std::string>(node->value)});
    }
}

// Processor 
std::map<std::string,long long> symbols;
std::map<int,long long> registers;

void execute_code(const std::vector<Insn>& code) {
    for (auto &ins : code) {
        switch(ins.op) {
            case Instruction::LOAD: registers[ins.reg] = std::get<long long>(ins.operand); break;
            case Instruction::LOAD_VAR: registers[ins.reg] = symbols[std::get<std::string>(ins.operand)]; break;
            case Instruction::STORE: symbols[std::get<std::string>(ins.operand)] = registers[ins.reg]; break;
            case Instruction::ADD: registers[ins.reg] += registers[std::get<int>(ins.operand)]; break;
            case Instruction::SUB: registers[ins.reg] -= registers[std::get<int>(ins.operand)]; break;
            case Instruction::MUL: registers[ins.reg] *= registers[std::get<int>(ins.operand)]; break;
            case Instruction::DIV: registers[ins.reg] /= registers[std::get<int>(ins.operand)]; break;
            case Instruction::MOD: registers[ins.reg] %= registers[std::get<int>(ins.operand)]; break;
        }
    }
}


int main() {
    std::string input;
    std::cout << "Enter code (e.g., a=5;b=2;a+6*b):\n";
    std::getline(std::cin, input);

    auto tokens = tokenize(input);
    size_t pos = 0;
    std::vector<Node*> statements;
    while (pos < tokens.size()) statements.push_back(parse_statement(tokens, pos));

    std::vector<Insn> code;
    std::map<std::string,int> var_map;

    for (auto stmt : statements) generate(stmt, code, 0, var_map); 

    execute_code(code);

    std::cout << "Result in R0: " << registers[0] << "\n";

    return 0;
}