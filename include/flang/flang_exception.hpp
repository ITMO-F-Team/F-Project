#pragma once

#include <stdexcept>
#include <string>

namespace flang {
class flang_exception : public std::runtime_error {
 public:
  explicit flang_exception(std::string const& e_msg) : std::runtime_error(e_msg) {}
};

class tokenizer_exception : public flang_exception {
 public:
  explicit tokenizer_exception(std::string const& e_msg) : flang_exception(e_msg) {}
};
}  // namespace flang