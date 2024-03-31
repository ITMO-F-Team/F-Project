#pragma once

#include <flang/parse/ast.hpp>
#include <functional>
#include <type_traits>
#include <variant>

namespace flang {

class IntegerValue;
class BuiltinFuncValue;

using Value = std::variant<IntegerValue, BuiltinFuncValue>;

class IntegerValue {
 public:
  explicit IntegerValue(int value) : _value(value) {}

  virtual ~IntegerValue() {}

  int value() const { return _value; }

 private:
  int _value;
};

using BuiltinFuncImpl = std::function<Value(std::vector<Value> args)>;

class BuiltinFuncValue {
 public:
  explicit BuiltinFuncValue(BuiltinFuncImpl impl) : _impl(impl) {}

  virtual ~BuiltinFuncValue() {}

  Value call(std::vector<Value> args) { return _impl(args); }

 private:
  BuiltinFuncImpl _impl;
};

// We must be able to copy values
// to pass them to user functions, etc.
static_assert(std::is_copy_constructible_v<Value>);

}  // namespace flang
