#include "eval_visitor.hpp"

#include <__utility/pair.h>

#include <flang/eval/value.hpp>
#include <flang/flang_exception.hpp>
#include <flang/parse/ast.hpp>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "builtins.hpp"

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
  auto result = this->loadVariable(name);
  _result = result;
}

void EvalVisitor::visitIntegerLiteral(IntegerLiteralNode const& node) { _result = IntegerValue(node.value()); }

void EvalVisitor::visitRealLiteral(RealLiteralNode const& node) { throw not_implemented_exception(""); }

void EvalVisitor::visitPureList(PureListNode const& node) {
  std::list<Value> elements;
  for (auto& el : node.elements()) {
    elements.emplace_back(this->visitElement(el));
  }
  _result = ListValue(elements);
}

void EvalVisitor::callUserFunc(UserFuncValue user_func, std::vector<Value> args) {
  auto formal_args = user_func.args();
  if (args.size() != formal_args.size()) {
    throw runtime_exception("Invalid number of arguments");
  }
  // Assign values to formal arguments
  for (int i = 0; i < args.size(); i++) {
    auto name = formal_args.at(i)->name();
    this->storeVariable(name, args[i]);
  }
  // Eval body
  auto body = user_func.body();
  try {
    this->visitElement(body);
    // If function does not return, we return `null`
    _result = _null_singleton;
  } catch (flang::return_control_flow_exception const& e) {
    // Here we do nothing hehe.
  }
}

void EvalVisitor::call(Value callee, std::vector<Value> args) {
  if (++cur_callstack_length_ > 100) {
    throw flang_exception("Stack overflow!");
  }
  if (const auto* f = std::get_if<UserFuncValue>(&callee)) {
    callUserFunc(*f, args);
  } else if (const auto* f = std::get_if<BuiltinFuncValue>(&callee)) {
    _result = f->call(args);
  } else {
    throw runtime_exception("You can only call callables");
  }
}

void EvalVisitor::visitCall(CallNode const& node) {
  // Eval callee
  auto fn = this->visitElement(node.callee());
  // Eval args
  std::vector<Value> args;
  for (auto& arg_expr : node.args()) {
    args.emplace_back(this->visitElement(arg_expr));
  }
  // Eval call
  call(fn, args);
}

void EvalVisitor::visitQuote(QuoteNode const& node) { throw not_implemented_exception(""); }

void EvalVisitor::visitSetq(SetqNode const& node) {
  auto name = node.name().name();
  auto value = this->visitElement(node.value());
  this->storeVariable(name, value);
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
  storeVariable(name, f);
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

void EvalVisitor::visitProg(ProgNode const& node) { this->visitElement(node.body()); }

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
    storeVariable(builtin.name, builtin.value);
  }
}

Value EvalVisitor::loadVariable(std::string const& varname) {
  if (_variables.contains(varname)) {
    return _variables.at(varname);
  } else {
    this->runtimeError("Variable '" + varname + "' is not defined");
    return _null_singleton;  // TODO: unreachable
  }
}

void EvalVisitor::storeVariable(std::string& varname, Value value) { _variables.insert_or_assign(varname, value); }

void EvalVisitor::runtimeError(std::string msg) { throw runtime_exception(msg); }

}  // namespace flang
