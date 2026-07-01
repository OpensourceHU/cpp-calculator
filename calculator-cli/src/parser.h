#pragma once

#include "lexer.h"
#include <vector>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    double parse();

private:
    std::vector<Token> tokens_;
    size_t pos_;

    Token current_token();
    void consume(TokenType type, const std::string& error_message);
    
    double expression();
    double term();
    double factor();
};
