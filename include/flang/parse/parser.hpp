#pragma once

#include <flang/parse/ast.hpp>
#include <flang/tokenize/token.hpp>
#include <memory>
#include <vector>

namespace flang {

std::unique_ptr<ProgramNode> parse(std::vector<Token>& tokens);

}
