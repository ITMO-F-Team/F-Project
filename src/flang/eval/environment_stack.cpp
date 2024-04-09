#include "environment_stack.h"

#include <flang/flang_exception.hpp>
#include <optional>
#include <stdexcept>

namespace flang {

void EnvironmentStack::pushEnvironment() {
  if (environments_stack_.size() == MAX_STACK_SIZE) {
    throw runtime_exception("Stack overflow!");
  }
  Environment empty_scope;
  environments_stack_.push_front(empty_scope);
}

void EnvironmentStack::popEnvironment() {
  if (environments_stack_.size() == 1) {
    throw std::runtime_error("Cannot pop global callframe");
  }
  environments_stack_.pop_front();
}

std::optional<Value> EnvironmentStack::loadVariable(std::string variable_name) {
  for (auto& env : environments_stack_) {
    if (env.contains(variable_name)) {
      return env.at(variable_name);
    }
  }
  return std::nullopt;
}

void EnvironmentStack::storeVariable(std::string variable_name, Value value) {
  auto& env = environments_stack_.front();
  env.insert_or_assign(variable_name, value);
}

}  // namespace flang
