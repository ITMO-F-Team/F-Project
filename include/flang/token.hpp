#pragma once

#include <string>

#include "token_type.hpp"

namespace flang {
class Token {
 public:
  Token(TokenType type, std::string value);

 private:
  TokenType type_;
  std::string value_;
};
}  // namespace flang