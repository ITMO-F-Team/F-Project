#include <nlohmann/json.hpp>

#include <flang/parse/ast.hpp>

using json = nlohmann::ordered_json;

namespace flang{
void print_identifier(IdentifierNode const& node, json& output);

void print_call_node(CallNode const& node, json& output);

void print_integer_literal(IntegerLiteralNode const& node, json& output);

void print_real_literal(RealLiteralNode const& node, json& output);

void print_setq(SetqNode const& node, json& output);

void print_cond(CondNode const& node, json& output);

void print_while(WhileNode const& node, json& output);

void print_pure_list(PureListNode const& node, json& output);

void print_prog(ProgNode const& node, json& output);

void print_func(FuncNode const& node, json& output);

void print_return(ReturnNode const& node, json& output);

void print_lambda(LambdaNode const& node, json& output);

void print_quote(QuoteNode const& node, json& output);

void print_break(BreakNode const& node, json& output);
}