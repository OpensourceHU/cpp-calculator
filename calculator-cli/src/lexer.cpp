#include "lexer.h"
#include <stdexcept>
#include <cctype>

Lexer::Lexer(std::string input) : input_(std::move(input)), pos_(0) {}

char Lexer::current_char() {
    if (pos_ >= input_.length()) {
        return '\0';
    }
    return input_[pos_];
}

void Lexer::advance() {
    pos_++;
}

void Lexer::skip_whitespace() {
    while (current_char() != '\0' && std::isspace(static_cast<unsigned char>(current_char()))) {
        advance();
    }
}

Token Lexer::read_number() {
    std::string val;
    size_t start_pos = pos_;
    bool has_dot = false;

    while (current_char() != '\0' && (std::isdigit(static_cast<unsigned char>(current_char())) || current_char() == '.')) {
        if (current_char() == '.') {
            if (has_dot) {
                throw std::runtime_error("Invalid number: multiple decimal points at position " + std::to_string(pos_));
            }
            has_dot = true;
        }
        val += current_char();
        advance();
    }
    return Token{TokenType::NUMBER, val, start_pos};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (current_char() != '\0') {
        skip_whitespace();
        char c = current_char();
        if (c == '\0') break;

        size_t start_pos = pos_;

        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            tokens.push_back(read_number());
        } else if (c == '+') {
            tokens.push_back(Token{TokenType::PLUS, "+", start_pos});
            advance();
        } else if (c == '-') {
            tokens.push_back(Token{TokenType::MINUS, "-", start_pos});
            advance();
        } else if (c == '*') {
            tokens.push_back(Token{TokenType::MUL, "*", start_pos});
            advance();
        } else if (c == '/') {
            tokens.push_back(Token{TokenType::DIV, "/", start_pos});
            advance();
        } else if (c == '(') {
            tokens.push_back(Token{TokenType::LPAREN, "(", start_pos});
            advance();
        } else if (c == ')') {
            tokens.push_back(Token{TokenType::RPAREN, ")", start_pos});
            advance();
        } else {
            throw std::runtime_error(std::string("Unexpected character '") + c + "' at position " + std::to_string(start_pos));
        }
    }

    tokens.push_back(Token{TokenType::END, "", pos_});
    return tokens;
}
