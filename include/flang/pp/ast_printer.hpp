#pragma once

#include "flang/parse/ast.hpp"
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>

namespace flang
{
class AstPrinter : public Visitor
{
public:
    explicit AstPrinter(std::ostream& os)
        : os_(os)
    {
    }

    AstPrinter()
        : os_(std::cout)
    {
    }

    void visitElement(std::shared_ptr<Element> node);
    void visitIdentifier(std::shared_ptr<Identifier> node) override;
    void visitInteger(std::shared_ptr<Integer> node) override;
    void visitReal(std::shared_ptr<Real> node) override;
    void visitBoolean(std::shared_ptr<Boolean> node) override;
    void visitNull(std::shared_ptr<Null> node) override;
    void visitList(std::shared_ptr<List> node) override;
    void visitFunction(std::shared_ptr<Function> node) override;

private:
    std::ostream& os_;
};

std::string printElement(std::shared_ptr<Element> const& node);

} // namespace flang
