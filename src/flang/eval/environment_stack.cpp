
#include <flang/flang_exception.hpp>
#include <flang/parse/ast.hpp>
#include <memory>
#include <stdexcept>

#include "flang/eval/environment_stack.hpp"

namespace flang
{

void EnvironmentStack::pushEnvironment()
{
    if (environment_stack_.size() == MAX_STACK_SIZE) {
        throwRuntimeError("Stack overflow!");
    }
    Environment empty_scope;
    environment_stack_.push_front(empty_scope);
}

void EnvironmentStack::popEnvironment()
{
    if (environment_stack_.size() == 1) {
        throw std::runtime_error("Cannot pop global environment");
    }
    environment_stack_.pop_front();
}

std::shared_ptr<Element> EnvironmentStack::loadVariable(std::string name)
{
    for (auto const& env : environment_stack_) {
        if (env.contains(name)) {
            return env.at(name);
        }
    }
    return nullptr;
}

void EnvironmentStack::storeVariable(std::string name, std::shared_ptr<Element> value)
{
    auto& env = environment_stack_.front();
    env.insert_or_assign(name, value);
}

void EnvironmentStack::throwRuntimeError(std::string message)
{
    throw flang_runtime_error(message);
}

} // namespace flang
