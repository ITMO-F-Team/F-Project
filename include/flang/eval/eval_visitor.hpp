#pragma once

#include "environment_stack.hpp"
#include <flang/parse/ast.hpp>
#include <memory>
#include <string>

namespace flang
{
class EvalVisitor : public Visitor
{
public:
    std::shared_ptr<Element> evalElement(std::shared_ptr<Element> node);
    void visitIdentifier(std::shared_ptr<Identifier> node) override;
    void visitInteger(std::shared_ptr<Integer> node) override;
    void visitReal(std::shared_ptr<Real> node) override;
    void visitBoolean(std::shared_ptr<Boolean> node) override;
    void visitNull(std::shared_ptr<Null> node) override;
    void visitList(std::shared_ptr<List> node) override;
    void visitFunction(std::shared_ptr<Function> node) override;
    void setResult(std::shared_ptr<Element> element);
    std::shared_ptr<Element> loadVariable(std::string name);
    void storeVariable(std::string name, std::shared_ptr<Element> element);
    void throwRuntimeError(std::string message);
    std::shared_ptr<Function> requireFunction(std::shared_ptr<Element> element);

private:
    EnvironmentStack env_;
    std::shared_ptr<Element> result_;
};
} // namespace flang
