#include "flang/eval/builtins.hpp"
#include <flang/parse/ast.hpp>
#include <flang/pp/ast_printer.hpp>
#include <memory>
#include <stdexcept>
#include <vector>


namespace flang
{

void print_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    for (auto& arg : args) {
        std::cout << printElement(visitor->evalElement(arg));
    }
}

std::vector<std::shared_ptr<Builtin>> BuiltinsRegistry::getAllBuiltins()
{
    std::vector<std::shared_ptr<Builtin>> result;
    for (auto const& entry : registry_) {
        result.emplace_back(std::make_shared<Builtin>(entry.first));
    }
    return result;
}

void BuiltinsRegistry::callBuiltin(std::shared_ptr<Builtin> builtin, std::vector<std::shared_ptr<Element>> args)
{
    if (!registry_.contains(builtin->getName())) {
        throw std::runtime_error("Builtin not found: " + builtin->getName());
    }
    auto impl = registry_.at(builtin->getName());
    impl(visitor_, args);
}

void BuiltinsRegistry::registerAllBuiltins()
{
    registry_.insert_or_assign("print", print_impl);
}

} // namespace flang
