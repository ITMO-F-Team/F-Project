#pragma once

#include <flang/eval/value.hpp>
#include <list>
#include <map>
#include <optional>
#include <string>
namespace flang {

const int MAX_STACK_SIZE = 1000;

class EnvironmentStack {
public:
  EnvironmentStack() : environments_stack_() {
    Environment empty_global_map;
    environments_stack_.push_back(empty_global_map);
  }

  void pushEnvironment();
  void popEnvironment();
  std::optional<Value> loadVariable(std::string variable_name);
  void storeVariable(std::string variable_name, Value value);

private:
  using Environment = std::map<std::string, Value>;
  std::list<Environment> environments_stack_;
};

class ScopedEnvironment {
public:
  ScopedEnvironment(EnvironmentStack environment_stack)
      : environment_stack_(environment_stack) {
    environment_stack_.pushEnvironment();
  }
  ~ScopedEnvironment() { environment_stack_.popEnvironment(); }

private:
  EnvironmentStack environment_stack_;
};

} // namespace flang
