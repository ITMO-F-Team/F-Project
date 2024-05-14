#include "flang/parse/ast.hpp"

namespace flang
{

void Visitor::visitProgram(Program program)
{
    for (auto& node : program) {
        node->accept(*this);
    }
}

} // namespace flang