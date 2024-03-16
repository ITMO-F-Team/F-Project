#include "flang/tokenize/token.hpp"

namespace flang {
Token::Token(TokenType type, std::string value) : type_(type), value_(std::move(value)) {}

std::string const& Token::value() const { return value_; }

std::string Token::value() { return value_; }

TokenType Token::type() const { return type_; }
}  // namespace flang
