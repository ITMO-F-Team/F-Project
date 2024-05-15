#include "flang/pp/ast_printer.hpp"
#include <flang/parse/ast.hpp>

namespace flang
{
void AstPrinter::visitElement(std::shared_ptr<Element> node)
{
    node->accept(*this);
}

void AstPrinter::visitBoolean(std::shared_ptr<Boolean> node)
{
    os_ << node->getValue();
}

void AstPrinter::visitIdentifier(std::shared_ptr<Identifier> node)
{
    os_ << node->getName();
}

void AstPrinter::visitInteger(std::shared_ptr<Integer> node)
{
    os_ << node->getValue();
}

void AstPrinter::visitReal(std::shared_ptr<Real> node)
{
    os_ << node->getValue();
}

void AstPrinter::visitNull(std::shared_ptr<Null> node)
{
    os_ << "null";
}

void AstPrinter::visitList(std::shared_ptr<List> node)
{
    for (auto& el : node->getElements()) {
        visitElement(el);
    }
}

void AstPrinter::visitUserFunction(std::shared_ptr<UserFunction> node)
{
    os_ << "<function " << node->getName() << ">";
}

void AstPrinter::visitBuiltin(std::shared_ptr<Builtin> node)
{
    os_ << "<built-in function " << node->getName() << ">";
}

std::string printElement(std::shared_ptr<Element> const& node)
{
    std::ostringstream oss;
    AstPrinter(oss).visitElement(node);
    return oss.str();
}
} // namespace flang
