#pragma once

#include <string>

namespace flang {
class Token {
 public:
  explicit Token(std::string value);

 private:
  std::string value_;
};
}  // namespace flang