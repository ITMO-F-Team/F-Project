#pragma once

#include <stdexcept>
#include <string>

namespace flang {

class not_implemented_exception : public std::runtime_error {
 public:
  explicit not_implemented_exception(std::string const& e_msg) : std::runtime_error("NOT_IMPLEMENTED: " + e_msg) {}
};

class flang_exception : public std::runtime_error {
 public:
  explicit flang_exception(std::string const& e_msg) : std::runtime_error(e_msg) {}
};

class tokenizer_exception : public flang_exception {
 public:
  explicit tokenizer_exception(std::string const& e_msg) : flang_exception(e_msg) {}
};

class parser_exception : public flang_exception {
 public:
  explicit parser_exception(std::string const& e_msg) : flang_exception(e_msg) {}
};

class runtime_exception : public flang_exception {
 public:
  explicit runtime_exception(std::string const& e_msg) : flang_exception(e_msg) {}
};

}  // namespace flang
