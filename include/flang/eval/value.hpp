#pragma once

#include <flang/parse/ast.hpp>
#include <functional>
#include <list>
#include <memory>
#include <type_traits>
#include <variant>
#include <vector>

namespace flang {

class NullValue;
class IntegerValue;
class BuiltinFuncValue;
class BoolValue;
class ListValue;

using Value = std::variant<NullValue, IntegerValue, BuiltinFuncValue, BoolValue, ListValue>;

// TODO: probably should be a singleton
class NullValue {
 public:
  explicit NullValue() {}
};

class IntegerValue {
 public:
  explicit IntegerValue(int value) : _value(value) {}

  int value() const { return _value; }

 private:
  int _value;
};

class BoolValue {
 public:
  explicit BoolValue(bool value) : _value(value) {}

  bool value() const { return _value; }

 private:
  bool _value;
};

class ListValue {
 public:
  explicit ListValue(std::list<Value> list) : _value(std::make_shared<std::list<Value>>(list)) {}

  std::shared_ptr<std::list<Value>> value() const { return _value; }

 private:
  // 🤓 Haha we cannot track cyclic links.
  std::shared_ptr<std::list<Value>> _value;
};

using BuiltinFuncImpl = std::function<Value(std::vector<Value> args)>;

class BuiltinFuncValue {
 public:
  explicit BuiltinFuncValue(BuiltinFuncImpl impl) : _impl(impl) {}

  Value call(std::vector<Value> args) { return _impl(args); }

 private:
  BuiltinFuncImpl _impl;
};

// We must be able to copy values
// to pass them to user functions, etc.
static_assert(std::is_copy_constructible_v<Value>);

}  // namespace flang
