#pragma once

#include <vector>

#include "ast.hpp"
#include "flang/tokenize/token.hpp"

namespace flang
{
Program parse(std::vector<Token> const& tokens);
}
