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

Value not_impl(std::vector<Value> args) {
  auto b = std::get<BoolValue>(args[0]);
  return BoolValue(!b.value());
}

bool equal_internal_impl(std::vector<Value> args) {
  auto left = args[0];
  auto right = args[1];
  if (const auto* left_i = std::get_if<IntegerValue>(&left)) {
    if (const auto* right_i = std::get_if<IntegerValue>(&right)) {
      return left_i->value() == right_i->value();
    } else {
      return false;
    }
  } else if (const auto* left_b = std::get_if<BoolValue>(&left)) {
    if (const auto* right_b = std::get_if<BoolValue>(&right)) {
      return left_b->value() == right_b->value();
    } else {
      return false;
    }
  } else {
    throw not_implemented_exception("");
  }
}

Value equal_impl(std::vector<Value> args) { return BoolValue(equal_internal_impl(args)); }

Value nonequal_impl(std::vector<Value> args) { return BoolValue(!equal_internal_impl(args)); }

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
  result.emplace_back(std::string("add"), BuiltinFuncValue("add", add_impl));
  result.emplace_back(std::string("and"), BuiltinFuncValue("and", and_impl));
  result.emplace_back(std::string("not"), BuiltinFuncValue("not", not_impl));
  result.emplace_back(std::string("equal"), BuiltinFuncValue("equal", equal_impl));
  result.emplace_back(std::string("nonequal"), BuiltinFuncValue("nonequal", nonequal_impl));

  result.emplace_back(std::string("print"), BuiltinFuncValue("print", println_impl));
  result.emplace_back(std::string("println"), BuiltinFuncValue("println", println_impl));
  result.emplace_back(std::string("assert"), BuiltinFuncValue("assert", assert_impl));

  return result;
}

}  // namespace flang
