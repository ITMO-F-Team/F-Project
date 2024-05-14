#include "eval_visitor.hpp"

#include <algorithm>
#include <flang/eval/value.hpp>
#include <flang/flang_exception.hpp>
#include <flang/parse/ast.hpp>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "builtins.hpp"
#include "environment_stack.h"

namespace flang {

Value EvalVisitor::visitElement(const ElementNode& node) {
  node.accept(*this);
  return _result;
}

Value EvalVisitor::visitElement(const std::unique_ptr<ElementNode>& node) {
  node->accept(*this);
  return _result;
}

Value EvalVisitor::visitElement(const std::shared_ptr<ElementNode>& node) {
  node->accept(*this);
  return _result;
}

void EvalVisitor::visitIdentifier(IdentifierNode const& node) {
  auto name = node.name();
  auto result = environment_stack_.loadVariable(name);
  if (result.has_value()) {
    _result = result.value();
  } else {
    throw runtime_exception("Variable '" + name + "' is not defined");
  }
}

void EvalVisitor::visitIntegerLiteral(IntegerLiteralNode const& node) { _result = IntegerValue(node.value()); }

void EvalVisitor::visitRealLiteral(RealLiteralNode const& node) { throw not_implemented_exception(""); }

void EvalVisitor::visitPureList(PureListNode const& node) {
  if (node.elements().empty()) {
    throw runtime_exception("Canont call empty list");
  }
  auto fn = this->visitElement(node.elements()[0]);

  std::vector<Value> args;
  std::transform(node.elements().begin() + 1, node.elements().end(), std::back_inserter(args),
                 [this](auto&& node) { return this->visitElement(node); });

  call(fn, args);
}

void EvalVisitor::callUserFunc(UserFuncValue user_func, std::vector<Value> args) {
  auto formal_args = user_func.args();
  if (args.size() != formal_args.size()) {
    throw runtime_exception("Invalid number of arguments");
  }
  // Assign values to formal arguments
  for (int i = 0; i < args.size(); i++) {
    auto name = formal_args.at(i)->name();
    environment_stack_.storeVariable(name, args[i]);
  }
  // Eval body
  auto body = user_func.body();
  try {
    _result = this->visitElement(body);
  } catch (flang::return_control_flow_exception const& e) {
    // Here we do nothing hehe.
  }
}

void EvalVisitor::call(Value callee, std::vector<Value> args) {
  ScopedEnvironment env(environment_stack_);

  if (const auto* f = std::get_if<UserFuncValue>(&callee)) {
    callUserFunc(*f, args);
  } else if (const auto* f = std::get_if<BuiltinFuncValue>(&callee)) {
    _result = f->call(args);
  } else {
    throw runtime_exception("You can only call callables");
  }
}

void EvalVisitor::visitQuote(QuoteNode const& node) { throw not_implemented_exception(""); }

void EvalVisitor::visitSetq(SetqNode const& node) {
  auto name = node.name().name();
  auto value = this->visitElement(node.value());
  environment_stack_.storeVariable(name, value);
}

void EvalVisitor::visitWhile(WhileNode const& node) {
  auto cond_value = this->visitElement(node.condition());
  while (std::get<BoolValue>(cond_value).value()) {
    try {
      this->visitElement(node.body());
    } catch (flang::break_control_flow_exception const& e) {
      break;
    }
  }
  _result = _null_singleton;
}

void EvalVisitor::visitReturn(ReturnNode const& node) {
  _result = visitElement(node.value());
  throw return_control_flow_exception();
}

void EvalVisitor::visitBreak(BreakNode const& node) { throw break_control_flow_exception(); }

void EvalVisitor::visitFunc(FuncNode const& node) {
  auto name = node.name().name();
  auto f = UserFuncValue(name, node.args(), node.body());
  environment_stack_.storeVariable(name, f);
}

std::string lambdaName(LambdaNode const& node) {
  std::string result = "";
  for (auto arg : node.args()) {
    auto arg_name = arg->name();
    if (!result.empty()) {
      result += ", ";
    }
    result += arg_name;
  }
  return "<lambda (" + result + ") ...>";
}

void EvalVisitor::visitLambda(LambdaNode const& node) {
  auto name = lambdaName(node);
  auto f = UserFuncValue(name, node.args(), node.body());
  _result = f;
}

void EvalVisitor::visitProg(ProgNode const& node) {
  // local context
  this->visitElement(node.body());
}

void EvalVisitor::visitCond(CondNode const& node) {
  auto cond_value = std::get<BoolValue>(this->visitElement(node.condition()));
  if (cond_value.value()) {
    _result = this->visitElement(node.thenBranch());
  } else {
    if (node.elseBranch().has_value()) {
      _result = this->visitElement(node.elseBranch()->get());
    } else {
      _result = _null_singleton;
    }
  }
}

void EvalVisitor::visitProgram(ProgramNode const& node) {
  try {
    for (auto& el : node.elements()) {
      this->visitElement(el);
    }
  } catch (flang::return_control_flow_exception const& e) {
    runtimeError("Out-of-function return");
  } catch (flang::break_control_flow_exception const& e) {
    runtimeError("Out-of-loop break");
  }
}

void EvalVisitor::setBuiltinValues() {
  for (auto builtin : getAllBuiltins()) {
    environment_stack_.storeVariable(builtin.name, builtin.value);
  }
}

void EvalVisitor::runtimeError(std::string msg) { throw runtime_exception(msg); }

}  // namespace flang
