#include "flang/token.hpp"

namespace flang {
Token::Token(std::string value) : value_(std::move(value)) {}
}  // namespace flang