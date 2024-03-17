#pragma once

#include <flang/parse/ast.hpp>
#include <iostream>
#include <vector>

#include "flang/tokenize/token.hpp"

namespace flang {
std::ostream& operator<<(std::ostream& os, ProgramNode const& program);

}  // namespace flang
