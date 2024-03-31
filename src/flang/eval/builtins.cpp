#include "builtins.hpp"

#include <flang/eval/value.hpp>
#include <iostream>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace flang {

Value add_impl(std::vector<Value> args) {
  // TODO: this will crash any moment
  auto n1 = std::get<IntegerValue>(args[0]);
  auto n2 = std::get<IntegerValue>(args[1]);
  return IntegerValue(n1.value() + n2.value());
}

Value println_impl(std::vector<Value> args) {
  auto n = std::get<IntegerValue>(args[0]);
  std::cout << n.value() << std::endl;
  return IntegerValue(0);  // TODO: should return a value of None
}

std::vector<Builtin> getAllBuiltins() {
  std::vector<Builtin> result;

  // add := integer addition
  result.emplace_back(std::string("add"), BuiltinFuncValue(add_impl));
  // println
  result.emplace_back(std::string("println"), BuiltinFuncValue(println_impl));
  // print is a synonym to println
  result.emplace_back(std::string("print"), BuiltinFuncValue(println_impl));

  return result;
}

}  // namespace flang
