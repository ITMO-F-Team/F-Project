#pragma once

#include <flang/eval/value.hpp>

namespace flang {

struct Builtin {
  std::string name;
  Value value;

  Builtin(std::string name, Value value) : name(name), value(value) {}
};

std::vector<Builtin> getAllBuiltins();

}  // namespace flang
