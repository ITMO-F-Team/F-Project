#include "builtins.hpp"

#include <flang/eval/value.hpp>
#include <flang/flang_exception.hpp>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace flang {

// ----- Binary Operators -----

// --- Integer binary operators ---

template <auto Operator>
Value int_binop_impl(std::vector<Value> args) {
  auto n1 = std::get<IntegerValue>(args[0]);
  auto n2 = std::get<IntegerValue>(args[1]);
  auto result = Operator(n1.value(), n2.value());
  return IntegerValue(result);
}

int add(int x, int y) { return x + y; }

int minus(int x, int y) { return x - y; }

int times(int x, int y) { return x * y; }

int divide(int x, int y) { return x / y; }

// --- Integer comparison operators ---

template <auto Operator>
Value int_comparison_binop_impl(std::vector<Value> args) {
  auto n1 = std::get<IntegerValue>(args[0]);
  auto n2 = std::get<IntegerValue>(args[1]);
  auto result = Operator(n1.value(), n2.value());
  return BoolValue(result);
}

bool less(int a, int b) { return a < b; }

bool lesseq(int a, int b) { return a <= b; }

bool greater(int a, int b) { return a > b; }

bool greatereq(int a, int b) { return a >= b; }

// --- Bolean binary operators ---

template <auto Operator>
Value bool_binop_impl(std::vector<Value> args) {
  auto b1 = std::get<BoolValue>(args[0]);
  auto b2 = std::get<BoolValue>(args[1]);
  auto result = Operator(b1.value(), b2.value());
  return BoolValue(result);
}

bool bool_and(bool a, bool b) { return a && b; }

bool bool_or(bool a, bool b) { return a || b; }

Value bool_not_impl(std::vector<Value> args) {
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

void builtin_function(std::vector<Builtin>& builtins, std::string name, BuiltinFuncImpl impl) {
  builtins.emplace_back(name, BuiltinFuncValue(name, impl));
}

std::vector<Builtin> getAllBuiltins() {
  std::vector<Builtin> result;

  // --- Simple Values ---
  result.emplace_back(std::string("true"), BoolValue(true));
  result.emplace_back(std::string("false"), BoolValue(false));

  // --- Functions ---
  // Integer binary operators
  builtin_function(result, "plus", int_binop_impl<add>);
  builtin_function(result, "minus", int_binop_impl<minus>);
  builtin_function(result, "times", int_binop_impl<times>);
  builtin_function(result, "divide", int_binop_impl<divide>);
  // Boolean operators
  builtin_function(result, "and", bool_binop_impl<bool_and>);
  builtin_function(result, "or", bool_binop_impl<bool_or>);
  builtin_function(result, "not", bool_not_impl);
  // Comparison operators
  builtin_function(result, "greater", int_comparison_binop_impl<greater>);
  builtin_function(result, "greatereq", int_comparison_binop_impl<greatereq>);
  builtin_function(result, "less", int_comparison_binop_impl<less>);
  builtin_function(result, "lesseq", int_comparison_binop_impl<lesseq>);
  builtin_function(result, "equal", equal_impl);
  builtin_function(result, "nonequal", nonequal_impl);
  // IO
  builtin_function(result, "print", println_impl);
  builtin_function(result, "printlnt", println_impl);
  builtin_function(result, "assert", assert_impl);

  return result;
}

}  // namespace flang
