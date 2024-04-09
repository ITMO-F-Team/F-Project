#pragma once

#include <flang/parse/ast.hpp>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace flang {

class NullValue;
class IntegerValue;
class BuiltinFuncValue;
class BoolValue;
class ListValue;
class UserFuncValue;

using Value = std::variant<NullValue, IntegerValue, BuiltinFuncValue, BoolValue, ListValue, UserFuncValue>;

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

class UserFuncValue {
 public:
  explicit UserFuncValue(std::string name, FuncFormalArgs args, FuncBody body)
      : _name(name), _args(args), _body(body) {}

  // TODO: ^^^ does it copy the body?

  std::string name() const { return _name; }

  FuncFormalArgs args() const { return _args; }

  FuncBody body() const { return _body; }

 private:
  std::string _name;
  FuncFormalArgs _args;
  FuncBody _body;
};

using BuiltinFuncImpl = std::function<Value(std::vector<Value> args)>;

class BuiltinFuncValue {
 public:
  explicit BuiltinFuncValue(std::string name, BuiltinFuncImpl impl) : _name(name), _impl(impl) {}

  Value call(std::vector<Value> args) const { return _impl(args); }

 private:
  std::string _name;
  BuiltinFuncImpl _impl;
};

// We must be able to copy values
// to pass them to user functions, etc.
static_assert(std::is_copy_constructible_v<Value>);

}  // namespace flang
