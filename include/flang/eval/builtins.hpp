#pragma once

#include <functional>
#include <map>
#include <string>

#include "flang/parse/ast.hpp"


namespace flang
{

using BuiltinImpl = std::function<void(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)>;

class BuiltinsRegistry
{
public:
    std::vector<std::shared_ptr<Builtin>> getAllBuiltins();
    void callBuiltin(std::shared_ptr<Builtin> builtin, std::vector<std::shared_ptr<Element>> args);

private:
    EvalVisitor* visitor_;
    std::map<std::string, BuiltinImpl> registry_;

    void registerAllBuiltins();
};


} // namespace flang
