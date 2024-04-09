#include "flang/parse/ast.hpp"
#include "flang/tokenize/token.hpp"
#include "parser_impl.hpp"

namespace flang {

std::shared_ptr<ProgramNode> parse(std::vector<Token>& tokens) { return ParserImpl(tokens).parseProgram(); }
}  // namespace flang
