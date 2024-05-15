#include "flang/eval/builtins.hpp"
#include <algorithm>
#include <flang/parse/ast.hpp>
#include <flang/pp/ast_printer.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
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

void cond_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    if ((args.size() != 2) && (args.size() != 3)) {
        visitor->throwRuntimeError("cond expects 2-3 arguments");
    }
    auto cond = visitor->requireBoolean(visitor->evalElement(args[0]));
    if (cond->getValue()) {
        visitor->evalElement(args[1]);
    } else {
        if (args.size() == 3) {
            visitor->evalElement(args[2]);
        }
    }
}

void func_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 3);
    auto id        = visitor->requireIdentifier(args[0]);
    auto args_list = visitor->requireList(args[1])->getElements();
    auto body      = args[2];

    std::vector<std::string> formal_args;
    std::transform(args_list.begin(), args_list.end(), std::back_inserter(formal_args), [visitor](auto&& x) {
        auto id = visitor->requireIdentifier(x);
        return id->getName();
    });

    auto fn = std::make_shared<UserFunction>(id->getName(), formal_args, body);
    visitor->storeVariable(id->getName(), fn);
}

void return_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    visitor->setResult(visitor->evalElement(args[0]));
}

void while_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 2);
    auto cond = args[0];
    while (visitor->requireBoolean(visitor->evalElement(cond))->getValue()) {
        visitor->evalElement(args[1]);
    }
}

// ====== Builtins Registry =====

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
    // TODO [BUILTINS]: quote lambda prog break
    // TODO [PREDEFINED]: head tail cons; arithmetic; logic; eval
    registry_.insert_or_assign("print", print_impl);
    registry_.insert_or_assign("assert", assert_impl);
    registry_.insert_or_assign("setq", setq_impl);
    registry_.insert_or_assign("cond", cond_impl);
    registry_.insert_or_assign("func", func_impl);
    registry_.insert_or_assign("return", return_impl);
    registry_.insert_or_assign("while", while_impl);
}

} // namespace flang
