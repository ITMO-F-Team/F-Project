#include <algorithm>
#include <flang/eval/environment_stack.hpp>
#include <flang/flang_exception.hpp>
#include <flang/pp/ast_printer.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "flang/eval/eval_visitor.hpp"
#include "flang/parse/ast.hpp"

namespace flang
{

std::shared_ptr<Element> EvalVisitor::evalElement(std::shared_ptr<Element> node)
{
    node->accept(*this);
    return result_;
}

void EvalVisitor::visitIdentifier(std::shared_ptr<Identifier> node)
{
    setResult(loadVariable(node->getName()));
}

void EvalVisitor::visitInteger(std::shared_ptr<Integer> node)
{
    setResult(std::move(node));
}

void EvalVisitor::visitReal(std::shared_ptr<Real> node)
{
    setResult(std::move(node));
}

void EvalVisitor::visitBoolean(std::shared_ptr<Boolean> node)
{
    setResult(std::move(node));
}

void EvalVisitor::visitNull(std::shared_ptr<Null> node)
{
    setResult(std::move(node));
}

void EvalVisitor::visitList(std::shared_ptr<List> node)
{
    auto elements = node->getElements();
    // 0. Check for NIL
    if (elements.empty()) {
        // Try `(print ())` in gnu clisp 2.49.60
        // It prints NIL
        setResult(std::make_shared<Null>());
        return;
    }
    // 1. Collect args
    std::vector<std::shared_ptr<Element>> args;
    std::copy(elements.begin() + 1, elements.end(), std::back_inserter(args));
    // 2. Eval callee
    auto callee = evalElement(elements[0]);
    if (auto fn = std::dynamic_pointer_cast<UserFunction>(callee)) {
        callUserFunc(fn, args);
    } else if (auto b = std::dynamic_pointer_cast<Builtin>(callee)) {
        builtin_registry_->callBuiltin(b, args);
    } else {
        throwRuntimeError(printElement(callee) + " is not a function");
    }
}

void EvalVisitor::callUserFunc(std::shared_ptr<UserFunction> fn, std::vector<std::shared_ptr<Element>> args)
{
    // 1. Check arity
    auto expected_n_args = fn->getFormalArgs().size();
    auto actual_n_args   = args.size();
    if (expected_n_args != actual_n_args) {
        throwRuntimeError("Function " + fn->getName() + " expects " + std::to_string(expected_n_args) + " but got " + std::to_string(actual_n_args));
    }
    // 2. Eval args
    std::vector<std::shared_ptr<Element>> arg_values;
    std::transform(args.begin(), args.end(), std::back_inserter(arg_values), [this](auto&& x) { return evalElement(x); });
    // 3. Create callframe
    ScopedEnvironment env(env_);
    // 3. Assign arg values to arg names
    for (int i = 0; i < expected_n_args; i++) {
        storeVariable(fn->getFormalArgs()[i], arg_values[i]);
    }
    // 4. Execute function body, capturing return
    try {
        evalElement(fn->getBody());
    } catch (flang_return const& e) {
        // Return calls setResult,
        // so we dont have to do anything here
    } catch (flang_break const& e) {
        throwRuntimeError("Out-of-loop 'break' in function " + fn->getName());
    }
}

void EvalVisitor::visitUserFunction(std::shared_ptr<UserFunction> node)
{
    setResult(std::move(node));
}

void EvalVisitor::visitBuiltin(std::shared_ptr<Builtin> node)
{
    setResult(std::move(node));
}

void EvalVisitor::setResult(std::shared_ptr<Element> element)
{
    result_ = std::move(element);
}

void EvalVisitor::setNullResult()
{
    setResult(std::make_shared<Null>());
}

ScopedEnvironment EvalVisitor::createScopedEnvironment()
{
    return ScopedEnvironment(env_);
}

std::shared_ptr<Element> EvalVisitor::loadVariable(std::string const& name)
{
    auto result = env_.loadVariable(name);
    if (result == nullptr) {
        throwRuntimeError("variable not found " + name);
    }
    return result;
}

void EvalVisitor::storeVariable(std::string const& name, std::shared_ptr<Element> element)
{
    if (isReservedKeyword(name)) {
        throwRuntimeError(name + " is a reserved keyword, its value cannot be reassigned");
    }
    return env_.storeVariable(name, std::move(element));
}

void EvalVisitor::throwRuntimeError(std::string const& message)
{
    return env_.throwRuntimeError(message);
}

std::shared_ptr<Integer> EvalVisitor::requireInteger(std::shared_ptr<Element> element)
{
    auto result = std::dynamic_pointer_cast<Integer>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError(printElement(element) + " is not an integer");
    }
    return result;
}

std::shared_ptr<Real> EvalVisitor::requireReal(std::shared_ptr<Real> element)
{
    auto result = std::dynamic_pointer_cast<Real>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError(printElement(element) + " is not a real number");
    }
    return result;
}

std::shared_ptr<Boolean> EvalVisitor::requireBoolean(std::shared_ptr<Element> element)
{
    auto result = std::dynamic_pointer_cast<Boolean>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError(printElement(element) + " is not a boolean");
    }
    return result;
}

std::shared_ptr<List> EvalVisitor::requireList(std::shared_ptr<Element> element)
{
    auto result = std::dynamic_pointer_cast<List>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError(printElement(element) + " is not a list");
    }
    return result;
}

std::shared_ptr<Identifier> EvalVisitor::requireIdentifier(std::shared_ptr<Element> element)
{
    auto result = std::dynamic_pointer_cast<Identifier>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError(printElement(element) + " is not an identifier");
    }
    return result;
}

void EvalVisitor::requireArgsNumber(std::vector<std::shared_ptr<Element>> args, int n)
{
    auto actual_n = args.size();
    if (actual_n != n) {
        throwRuntimeError("Expected " + std::to_string(n) + " args, but got " + std::to_string(actual_n));
    }
}

void EvalVisitor::setAllBuiltins()
{
    for (auto builtin : builtin_registry_->getAllBuiltins()) {
        env_.storeVariable(builtin->getName(), builtin);
    }
}

} // namespace flang
