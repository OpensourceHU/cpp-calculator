#pragma once

#include <string>
#include <vector>

enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LPAREN,
    RPAREN,
    END
};

struct Token {
    TokenType type;
    std::string value;
    size_t position;
};

class Lexer {
public:
    explicit Lexer(std::string input);
    std::vector<Token> tokenize();

private:
    std::string input_;
    size_t pos_;
    char current_char();
    void advance();
    void skip_whitespace();
    Token read_number();
};
