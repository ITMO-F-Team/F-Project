#include "builtins.hpp"

#include <flang/eval/value.hpp>
#include <flang/flang_exception.hpp>
#include <iostream>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace flang {

// ----- Binary Operators -----

Value add_impl(std::vector<Value> args) {
  // TODO: this will crash any moment
  auto n1 = std::get<IntegerValue>(args[0]);
  auto n2 = std::get<IntegerValue>(args[1]);
  return IntegerValue(n1.value() + n2.value());
}

Value and_impl(std::vector<Value> args) {
  auto b1 = std::get<BoolValue>(args[0]);
  auto b2 = std::get<BoolValue>(args[1]);
  return BoolValue(b1.value() && b2.value());
}

// ----- IO -----

Value println_impl(std::vector<Value> args) {
  auto n = std::get<IntegerValue>(args[0]);
  std::cout << n.value() << std::endl;
  return NullValue();  // TODO: should return a value of None
}

Value assert_impl(std::vector<Value> args) {
  auto b = std::get<BoolValue>(args[0]);
  if (b.value()) {
    return NullValue();
  } else {
    throw runtime_exception("Assertion failed!");
  }
}

std::vector<Builtin> getAllBuiltins() {
  std::vector<Builtin> result;

  // --- Simple Values ---
  result.emplace_back(std::string("true"), BoolValue(true));
  result.emplace_back(std::string("false"), BoolValue(false));

  // --- Functions ---
  result.emplace_back(std::string("add"), BuiltinFuncValue(add_impl));
  result.emplace_back(std::string("and"), BuiltinFuncValue(and_impl));

  result.emplace_back(std::string("print"), BuiltinFuncValue(println_impl));
  result.emplace_back(std::string("println"), BuiltinFuncValue(println_impl));
  result.emplace_back(std::string("assert"), BuiltinFuncValue(assert_impl));

  return result;
}

}  // namespace flang
