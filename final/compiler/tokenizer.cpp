#include "tokenizer.hpp"
#include <cctype>
#include <stdexcept>

static bool isKeyword(const std::string &s){
    static const std::vector<std::string> kws = {"func","return","if","else","while","print"};
    for(auto &k:kws) if(k==s) return true; return false;
}

Tokenizer::Tokenizer(const std::string &src): s(src), i(0){}

void Tokenizer::skipWhitespace(){
    while(i<s.size()){
        char c = s[i];
        if(c=='/' && i+1<s.size() && s[i+1]=='/'){
            // line comment
            i+=2;
            while(i<s.size() && s[i]!='\n') i++;
        } else if(isspace((unsigned char)c)){
            i++; 
        } else break;
    }
}

Token Tokenizer::readNumber(){
    size_t start = i; while(i<s.size() && isdigit((unsigned char)s[i])) i++;
    std::string t = s.substr(start, i-start);
    Token tok; tok.type = TokenType::Number; tok.text = t; tok.value = std::stoi(t);
    return tok;
}

Token Tokenizer::readIdent(){
    size_t start = i; while(i<s.size() && (isalnum((unsigned char)s[i])|| s[i]=='_')) i++;
    std::string t = s.substr(start, i-start);
    Token tok; tok.text = t;
    if(isKeyword(t)) tok.type = TokenType::Keyword; else tok.type = TokenType::Ident;
    return tok;
}

Token Tokenizer::next(){
    skipWhitespace();
    if(i>=s.size()) return Token{TokenType::End, "", 0};
    char c = s[i];
    if(isdigit((unsigned char)c)) return readNumber();
    if(isalpha((unsigned char)c) || c=='_') return readIdent();
    // symbols and operators
    i++;
    std::string t(1,c);
    Token tok; tok.type = TokenType::Symbol; tok.text = t; tok.value = 0;
    // handle two-char symbols
    if(c=='=' && i<s.size() && s[i]=='=') { i++; tok.text = "=="; }
    else if(c=='!' && i<s.size() && s[i]=='=') { i++; tok.text = "!="; }
    else if(c=='<' && i<s.size() && s[i]=='=') { i++; tok.text = "<="; }
    else if(c=='>' && i<s.size() && s[i]=='=') { i++; tok.text = ">="; }
    return tok;
}

Token Tokenizer::peek(){
    size_t save = i;
    Token t = next();
    i = save;
    return t;
}

bool Tokenizer::eof(){
    size_t save = i; skipWhitespace(); bool r = (i>=s.size()); i = save; return r;
}
