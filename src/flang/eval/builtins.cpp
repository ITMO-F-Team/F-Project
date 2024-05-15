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
        auto x = visitor->evalElement(arg);
        std::cout << printElement(x);
    }
}

void assert_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    auto val = visitor->requireBoolean(visitor->evalElement(args[0]));
    if (!val->getValue()) {
        visitor->throwRuntimeError("Assertion error!");
    }
}

void setq_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 2);
    auto id  = visitor->requireIdentifier(args[0]);
    auto val = visitor->evalElement(args[1]);
    visitor->storeVariable(id->getName(), val);
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
    registry_.insert_or_assign("assert", assert_impl);
    registry_.insert_or_assign("setq", setq_impl);
}

} // namespace flang
