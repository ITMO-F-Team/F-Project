#pragma once

#include "environment_stack.hpp"
#include <flang/eval/builtins.hpp>
#include <flang/parse/ast.hpp>
#include <memory>
#include <string>
#include <vector>

namespace flang
{
class BuiltinsRegistry;

class EvalVisitor : public Visitor
{
public:
    EvalVisitor()
        : builtin_registry_(std::make_shared<BuiltinsRegistry>(this))
    {
        setAllBuiltins();
    }

    std::shared_ptr<Element> evalElement(std::shared_ptr<Element> node);
    void visitIdentifier(std::shared_ptr<Identifier> node) override;
    void visitInteger(std::shared_ptr<Integer> node) override;
    void visitReal(std::shared_ptr<Real> node) override;
    void visitBoolean(std::shared_ptr<Boolean> node) override;
    void visitNull(std::shared_ptr<Null> node) override;
    void visitList(std::shared_ptr<List> node) override;
    void visitUserFunction(std::shared_ptr<UserFunction> node) override;
    void visitBuiltin(std::shared_ptr<Builtin> node) override;

    // --- Evaluation State ---
    void setResult(std::shared_ptr<Element> element);
    std::shared_ptr<Element> loadVariable(std::string const& name);
    void storeVariable(std::string const& name, std::shared_ptr<Element> element);
    void throwRuntimeError(std::string const& message);

    // --- Requires ---
    std::shared_ptr<Integer> requireInteger(std::shared_ptr<Element> element);
    std::shared_ptr<Real> requireReal(std::shared_ptr<Real> element);
    std::shared_ptr<Boolean> requireBoolean(std::shared_ptr<Element> element);
    std::shared_ptr<List> requireList(std::shared_ptr<Element> element);
    std::shared_ptr<Identifier> requireIdentifier(std::shared_ptr<Element> element);
    void requireArgsNumber(std::vector<std::shared_ptr<Element>> args, int n);

private:
    EnvironmentStack env_;
    std::shared_ptr<Element> result_;
    std::shared_ptr<BuiltinsRegistry> builtin_registry_;

    void setAllBuiltins();
};
} // namespace flang
