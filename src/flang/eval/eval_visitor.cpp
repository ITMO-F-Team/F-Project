#include <algorithm>
#include <iterator>
#include <memory>
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
    // 1. Eval callee
    auto callee = requireFunction(evalElement(elements[0]));
    // 2. Collect args
    std::vector<std::shared_ptr<Element>> args;
    std::copy(elements.begin() + 1, elements.end(), std::back_inserter(args));
    // 3. Invoke
    callee->call(*this, std::move(args));
}

void EvalVisitor::visitFunction(std::shared_ptr<Function> node)
{
    setResult(std::move(node));
}

void EvalVisitor::setResult(std::shared_ptr<Element> element)
{
    result_ = std::move(element);
}

std::shared_ptr<Element> EvalVisitor::loadVariable(std::string const& name)
{
    return env_.loadVariable(name);
}

void EvalVisitor::storeVariable(std::string const& name, std::shared_ptr<Element> element)
{
    return env_.storeVariable(name, std::move(element));
}

void EvalVisitor::throwRuntimeError(std::string const& message)
{
    return env_.throwRuntimeError(message);
}

std::shared_ptr<Function> EvalVisitor::requireFunction(std::shared_ptr<Element> element)
{
    auto result = std::dynamic_pointer_cast<Function>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError("{} is not a function");
    }
    return result;
}

std::shared_ptr<Integer> EvalVisitor::requireInteger(std::shared_ptr<Element> element)
{
    auto result = std::dynamic_pointer_cast<Integer>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError("{} is not an integer");
    }
    return result;
}

std::shared_ptr<Real> EvalVisitor::requireReal(std::shared_ptr<Real> element)
{
    auto result = std::dynamic_pointer_cast<Real>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError("{} is not a real number");
    }
    return result;
}

std::shared_ptr<Boolean> EvalVisitor::requireBoolean(std::shared_ptr<Element> element)
{
    auto result = std::dynamic_pointer_cast<Boolean>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError("{} is not a boolean");
    }
    return result;
}

std::shared_ptr<List> EvalVisitor::requireList(std::shared_ptr<Element> element)
{
    auto result = std::dynamic_pointer_cast<List>(element);
    if (!result) {
        // TODO: print element
        throwRuntimeError("{} is not a list");
    }
    return result;
}

} // namespace flang
