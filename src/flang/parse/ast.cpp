#include "flang/parse/ast.hpp"

namespace flang
{

void Visitor::visitProgram(Program program)
{
    for (auto& node : program) {
        node->accept(*this);
    }
}

bool isReservedKeyword(std::string s)
{
    return s == "quote" || s == "setq" || s == "func" || s == "lambda" || s == "prog" || s == "cond" || s == "while" || s == "return" || s == "break";
}

} // namespace flang
