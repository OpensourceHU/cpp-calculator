#include "parser.h"
#include <stdexcept>
#include <string>

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)), pos_(0) {}

Token Parser::current_token() {
    if (pos_ >= tokens_.size()) {
        return Token{TokenType::END, "", 0};
    }
    return tokens_[pos_];
}

void Parser::consume(TokenType type, const std::string& error_message) {
    if (current_token().type == type) {
        pos_++;
    } else {
        throw std::runtime_error(error_message + " at position " + std::to_string(current_token().position));
    }
}

double Parser::parse() {
    if (tokens_.empty() || tokens_[0].type == TokenType::END) {
        throw std::runtime_error("Empty expression");
    }
    double result = expression();
    if (current_token().type != TokenType::END) {
        throw std::runtime_error("Unexpected token '" + current_token().value + "' at position " + std::to_string(current_token().position));
    }
    return result;
}

double Parser::expression() {
    double result = term();

    while (current_token().type == TokenType::PLUS || current_token().type == TokenType::MINUS) {
        TokenType op = current_token().type;
        pos_++; // consume operator
        double right = term();
        if (op == TokenType::PLUS) {
            result += right;
        } else {
            result -= right;
        }
    }

    return result;
}

double Parser::term() {
    double result = factor();

    while (current_token().type == TokenType::MUL || current_token().type == TokenType::DIV) {
        TokenType op = current_token().type;
        pos_++; // consume operator
        double right = factor();
        if (op == TokenType::MUL) {
            result *= right;
        } else {
            if (right == 0.0) {
                throw std::runtime_error("Division by zero error");
            }
            result /= right;
        }
    }

    return result;
}

double Parser::factor() {
    Token token = current_token();

    if (token.type == TokenType::MINUS) {
        pos_++;
        return -factor();
    } else if (token.type == TokenType::PLUS) {
        pos_++;
        return factor();
    } else if (token.type == TokenType::NUMBER) {
        pos_++;
        return std::stod(token.value);
    } else if (token.type == TokenType::LPAREN) {
        pos_++;
        double result = expression();
        consume(TokenType::RPAREN, "Expected matching ')'");
        return result;
    } else {
        throw std::runtime_error("Expected a number or '(' at position " + std::to_string(token.position));
    }
}
