#pragma once

#include "environment_stack.hpp"

namespace flang
{
class EvalVisitor : public Visitor
{
public:
    void visitIdentifier(std::shared_ptr<Identifier> node) override;
    void visitInteger(std::shared_ptr<Integer> node) override;
    void visitReal(std::shared_ptr<Real> node) override;
    void visitBoolean(std::shared_ptr<Boolean> node) override;
    void visitNull(std::shared_ptr<Null> node) override;
    void visitList(std::shared_ptr<List> node) override;
    void visitFunction(std::shared_ptr<Function> node) override;

private:
    EnvironmentStack env_;
    std::shared_ptr<Element> result_;
};
} // namespace flang
