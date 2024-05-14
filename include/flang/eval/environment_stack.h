#pragma once

#include <flang/ast_visitor.hpp>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
namespace flang {

const int MAX_STACK_SIZE = 1000;

class EnvironmentStack {
public:
  EnvironmentStack() : environment_stack_() {
    Environment empty_global_map;
    environment_stack_.push_back(empty_global_map);
  }

  void pushEnvironment();
  void popEnvironment();

  std::shared_ptr<Element> loadVariable(std::string name);
  void storeVariable(std::string name, std::shared_ptr<Element> element);

  void throwRuntimeError(std::string message);

private:
  using Environment = std::map<std::string, std::shared_ptr<Element>>;
  std::list<Environment> environment_stack_;
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
