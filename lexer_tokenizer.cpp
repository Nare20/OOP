#include <iostream>
#include <vector>
#include <string>
#include <cctype>

using namespace std;

enum class TokenType {
    NUM,
    IDENT,
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
        else {
            cout << "Unknown char: " << c << endl;
        }

        i++;
    }

    tokens.push_back({TokenType::END, "END"});
    return tokens;
}

string tokenName(TokenType t) {
    switch(t) {
        case TokenType::NUM: return "NUM";
        case TokenType::IDENT: return "IDENT";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MUL: return "MUL";
        case TokenType::DIV: return "DIV";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::SEMI: return "SEMI";
        case TokenType::END: return "END";
    }
    return "???";
}

int main() {
    string input;
    cout << "Input: ";
    getline(cin, input);

    auto tokens = tokenize(input);

    for (auto& t : tokens) {
        cout << tokenName(t.type) << " -> " << t.text << endl;
    }
}