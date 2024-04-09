#pragma once

#include <flang/parse/ast.hpp>
#include <memory>

namespace flang {

void eval(std::shared_ptr<ProgramNode> node);

}
