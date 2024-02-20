#pragma once

#include <string>

#include "token_type.hpp"

namespace flang {
class Token {
 public:
  Token(TokenType type, std::string value);

  std::string const& value() const;

  std::string value();

  TokenType type() const;

 private:
  TokenType type_;
  std::string value_;
};
}  // namespace flang
