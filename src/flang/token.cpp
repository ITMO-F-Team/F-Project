#include "flang/token.hpp"

namespace flang {
Token::Token(TokenType type, std::string value) : type_(type), value_(std::move(value)) {}
}  // namespace flang