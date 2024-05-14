#include "flang/eval/eval_visitor.hpp"
#include <flang/parse/ast.hpp>
#include <memory>

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
    setResult(node);
}

void EvalVisitor::visitReal(std::shared_ptr<Real> node)
{
    setResult(node);
}

void EvalVisitor::visitBoolean(std::shared_ptr<Boolean> node)
{
    setResult(node);
}

void EvalVisitor::visitNull(std::shared_ptr<Null> node)
{
    setResult(node);
}

void EvalVisitor::visitList(std::shared_ptr<List> node)
{
    // TODO: Call
}

void EvalVisitor::visitFunction(std::shared_ptr<Function> node)
{
    setResult(node);
}

void EvalVisitor::setResult(std::shared_ptr<Element> element)
{
    result_ = element;
}

std::shared_ptr<Element> EvalVisitor::loadVariable(std::string name)
{
    return env_.loadVariable(name);
}

void EvalVisitor::storeVariable(std::string name, std::shared_ptr<Element> element)
{
    return env_.storeVariable(name, element);
}

void EvalVisitor::throwRuntimeError(std::string message)
{
    return env_.throwRuntimeError(message);
}

} // namespace flang
