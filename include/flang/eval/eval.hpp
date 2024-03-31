#pragma once

#include <flang/parse/ast.hpp>
#include <memory>

namespace flang {

void eval(std::unique_ptr<ProgramNode> node);

}
