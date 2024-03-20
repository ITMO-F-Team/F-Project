#include "flang/tokenize/token.hpp"

namespace flang {
Token::Token(TokenType type, std::string value, size_t line, size_t col)
    : type_(type), value_(std::move(value)), location_({.line = line, .col = col}) {}

std::string const& Token::value() const { return value_; }

std::string Token::value() { return value_; }

TokenType Token::type() const { return type_; }

Token::TokenLocation Token::location() const { return location_; }
}  // namespace flang
