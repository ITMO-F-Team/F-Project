#include <flang/parse/ast.hpp>
#include <memory>

#include "eval_visitor.hpp"

namespace flang {

void eval(std::unique_ptr<ProgramNode> node) {
  auto v = EvalVisitor();
  node->accept(v);
}

}  // namespace flang
