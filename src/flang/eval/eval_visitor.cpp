#include "eval_visitor.hpp"

#include <flang/eval/value.hpp>
#include <flang/flang_exception.hpp>
#include <flang/parse/ast.hpp>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "builtins.hpp"

namespace flang {

void todo(std::string const& e_msg = "") { throw not_implemented_exception(e_msg); }

Value EvalVisitor::visitElement(const ElementNode& node) {
  node.accept(*this);
  return _result;
}

Value EvalVisitor::visitElement(const std::unique_ptr<ElementNode>& node) {
  node->accept(*this);
  return _result;
}

void EvalVisitor::visitIdentifier(IdentifierNode const& node) {
  auto name = node.name();
  auto result = this->loadVariable(name);
  _result = result;
}

void EvalVisitor::visitIntegerLiteral(IntegerLiteralNode const& node) { _result = IntegerValue(node.value()); }

void EvalVisitor::visitRealLiteral(RealLiteralNode const& node) { todo(); }

void EvalVisitor::visitPureList(PureListNode const& node) { todo(); }

void EvalVisitor::visitCall(CallNode const& node) {
  // Eval callee
  // TODO: this will crash any moment
  auto fn = std::get<BuiltinFuncValue>(this->visitElement(node.callee()));
  // Eval args
  std::vector<Value> args;
  for (auto& arg_expr : node.args()) {
    args.emplace_back(this->visitElement(arg_expr));
  }
  // Call
  _result = fn.call(args);
}

void EvalVisitor::visitQuote(QuoteNode const& node) { todo(); }

void EvalVisitor::visitSetq(SetqNode const& node) { todo(); }

void EvalVisitor::visitWhile(WhileNode const& node) { todo(); }

void EvalVisitor::visitReturn(ReturnNode const& node) { todo(); }

void EvalVisitor::visitBreak(BreakNode const& node) { todo(); }

void EvalVisitor::visitFunc(FuncNode const& node) { todo(); }

void EvalVisitor::visitLambda(LambdaNode const& node) { todo(); }

void EvalVisitor::visitProg(ProgNode const& node) { this->visitElement(node.body()); }

void EvalVisitor::visitCond(CondNode const& node) { todo(); }

void EvalVisitor::visitProgram(ProgramNode const& node) {
  for (auto& el : node.elements()) {
    this->visitElement(el);
  }
}

void EvalVisitor::setBuiltinValues() {
  for (auto builtin : getAllBuiltins()) {
    storeVariable(builtin.name, builtin.value);
  }
}

Value EvalVisitor::loadVariable(std::string& varname) {
  if (_variables.contains(varname)) {
    return _variables.at(varname);
  } else {
    this->runtimeError("Variable '" + varname + "' is not defined");
    return IntegerValue(0);  // TODO: unreachable
  }
}

void EvalVisitor::storeVariable(std::string& varname, Value value) { _variables.insert_or_assign(varname, value); }

void EvalVisitor::runtimeError(std::string msg) { throw runtime_exception(msg); }

}  // namespace flang
