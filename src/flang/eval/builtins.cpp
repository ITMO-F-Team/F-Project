#pragma once

#include "flang/eval/builtins.hpp"
#include <flang/parse/ast.hpp>
#include <memory>
#include <stdexcept>
#include <vector>


namespace flang
{


std::vector<std::shared_ptr<Builtin>> BuiltinsRegistry::getAllBuiltins() {
    std::vector<std::shared_ptr<Builtin>> result;
    // TODO:
    // return [Builtin(name) for name in registry_.keys()]
    return result;
}

void BuiltinsRegistry::callBuiltin(std::shared_ptr<Builtin> builtin, std::vector<std::shared_ptr<Element>> args) {
    if (!registry_.contains(builtin->getName())) {
        throw std::runtime_error("Builtin not found: " + builtin->getName());
    }
    auto impl = registry_.at(builtin->getName());
    impl(visitor_, args);
}


void BuiltinsRegistry::registerAllBuiltins() {
    // TODO: add builtins
}

} // namespace flang
