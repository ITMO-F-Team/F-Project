#pragma once

#include <string_view>
#include <vector>

#include "token.hpp"

namespace flang {
class Tokenizer {
 public:
  std::vector<Token> tokenize(std::string_view source);
};
}  // namespace flang