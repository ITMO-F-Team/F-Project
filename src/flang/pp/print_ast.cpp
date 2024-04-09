#include "flang/pp/print_ast.hpp"

#include <stdexcept>

#include "print_ast_impl.hpp"

using json = nlohmann::ordered_json;

namespace flang {

void print_element(ElementNode const& element, json& output) {
  switch (element.type()) {
    case ElementNodeType::Identifier:
      if (const IdentifierNode* identifier_node = dynamic_cast<IdentifierNode const*>(&element)) {
        print_identifier(*identifier_node, output);
      }
      break;
    case ElementNodeType::Call:
      if (const CallNode* call_node = dynamic_cast<CallNode const*>(&element)) {
        print_call_node(*call_node, output);
      }
      break;
    case ElementNodeType::IntegerLiteral:
      if (const IntegerLiteralNode* integer_node = dynamic_cast<IntegerLiteralNode const*>(&element)) {
        print_integer_literal(*integer_node, output);
      }
      break;
    case ElementNodeType::RealLiteral:
      if (const RealLiteralNode* real_node = dynamic_cast<RealLiteralNode const*>(&element)) {
        print_real_literal(*real_node, output);
      }
      break;
    case ElementNodeType::Setq:
      if (const SetqNode* setq_node = dynamic_cast<SetqNode const*>(&element)) {
        print_setq(*setq_node, output);
      }
      break;
    case ElementNodeType::Cond:
      if (const CondNode* cond_node = dynamic_cast<CondNode const*>(&element)) {
        print_cond(*cond_node, output);
      }
      break;
    case ElementNodeType::While:
      if (const WhileNode* while_node = dynamic_cast<WhileNode const*>(&element)) {
        print_while(*while_node, output);
      }
      break;
    case ElementNodeType::PureList:
      if (const PureListNode* pure_list_node = dynamic_cast<PureListNode const*>(&element)) {
        print_pure_list(*pure_list_node, output);
      }
      break;
    case ElementNodeType::Prog:
      if (const ProgNode* prog_node = dynamic_cast<ProgNode const*>(&element)) {
        print_prog(*prog_node, output);
      }
      break;
    case ElementNodeType::Func:
      if (const FuncNode* func_node = dynamic_cast<FuncNode const*>(&element)) {
        print_func(*func_node, output);
      }
      break;
    case ElementNodeType::Return:
      if (const ReturnNode* return_node = dynamic_cast<ReturnNode const*>(&element)) {
        print_return(*return_node, output);
      }
      break;
    case ElementNodeType::Lambda:
      if (const LambdaNode* lambda_node = dynamic_cast<LambdaNode const*>(&element)) {
        print_lambda(*lambda_node, output);
      }
      break;
    case ElementNodeType::Quote:
      if (const QuoteNode* quote_node = dynamic_cast<QuoteNode const*>(&element)) {
        print_quote(*quote_node, output);
      }
      break;
    case ElementNodeType::Break:
      if (const BreakNode* break_node = dynamic_cast<BreakNode const*>(&element)) {
        print_break(*break_node, output);
      }
      break;
    default:
      throw std::runtime_error("Not implemented");
  }
}

void print_break(BreakNode const& node, json& output) { output["break"] = {}; }

void print_quote(QuoteNode const& node, json& output) { print_element(node.arg(), output["quote"]); }

void print_lambda(LambdaNode const& node, json& output) {
  json lambda_output;
  lambda_output["lambda"] = {};

  if (node.args().empty()) {
    lambda_output["lambda"]["args"] = {};
  }

  for (const auto& arg : node.args()) {
    json arg_output;
    print_element(*arg, arg_output);
    lambda_output["lambda"]["args"].push_back(arg_output);
  }
  print_element(*node.body(), lambda_output["lambda"]["body"]);
  output.push_back(lambda_output);
}

void print_return(ReturnNode const& node, json& output) { print_element(node.value(), output["return"]); }

void print_func(FuncNode const& node, json& output) {
  json func_output;
  func_output["func"] = {};
  print_element(node.name(), func_output["func"]["name"]);

  if (node.args().empty()) {
    func_output["func"]["args"] = {};
  }

  for (const auto& arg : node.args()) {
    json arg_output;
    print_element(*arg, arg_output);
    func_output["func"]["args"].push_back(arg_output);
  }
  print_element(*node.body(), func_output["func"]["body"]);
  output.push_back(func_output);
}

void print_prog(ProgNode const& node, json& output) {
  json prog_output;
  prog_output["prog"] = {};

  if (node.args().empty()) {
    prog_output["prog"]["args"] = {};
  }

  for (const auto& arg : node.args()) {
    json arg_output;
    print_element(*arg, arg_output);
    prog_output["prog"]["args"].push_back(arg_output);
  }
  print_element(node.body(), prog_output["prog"]["body"]);
  output.push_back(prog_output);
}

void print_pure_list(PureListNode const& node, json& output) {
  json pure_list_output;
  pure_list_output["pure_list"] = {};
  for (const auto& item : node.elements()) {
    json item_output;
    print_element(*item, item_output);
    pure_list_output["pure_list"].push_back(item_output);
  }
  output.push_back(pure_list_output);
}

void print_while(WhileNode const& node, json& output) {
  json while_output;
  while_output["while"] = {};
  print_element(node.condition(), while_output["while"]["condition"]);
  print_element(node.body(), while_output["while"]["body"]);
  output.push_back(while_output);
}

void print_cond(CondNode const& node, json& output) {
  json cond_output;
  cond_output["cond"] = {};
  print_element(node.condition(), cond_output["cond"]["condition"]);
  print_element(node.thenBranch(), cond_output["cond"]["then_branch"]);
  if (node.elseBranch().has_value()) {
    print_element(node.elseBranch().value(), cond_output["cond"]["else_branch"]);
  } else {
    cond_output["cond"]["else_branch"] = {};
  }
  output.push_back(cond_output);
}

void print_setq(SetqNode const& node, json& output) {
  json setq_output;
  setq_output["setq"] = {};
  print_identifier(node.name(), setq_output["setq"]["name"]);
  print_element(node.value(), setq_output["setq"]["value"]);
  output.push_back(setq_output);
}

void print_identifier(IdentifierNode const& node, json& output) { output["identifier"] = node.name(); }

void print_integer_literal(IntegerLiteralNode const& node, json& output) { output["integer"] = node.value(); }

void print_real_literal(RealLiteralNode const& node, json& output) { output["real"] = node.value(); }

void print_call_node(CallNode const& node, json& output) {
  json call_output;
  call_output["call"] = {};
  print_identifier(node.callee(), call_output["call"]["name"]);

  if (node.args().empty()) {
    call_output["call"]["args"] = {};
  }

  for (const auto& arg : node.args()) {
    json arg_output;
    print_element(*arg.get(), arg_output);
    call_output["call"]["args"].push_back(arg_output);
  }
  output.push_back(call_output);
}

void print_prog(std::ostream& os, ProgramNode const& node) {
  json output;
  output["program"] = {};
  for (auto& el : node.elements()) {
    print_element(*el.get(), output["program"]);
  }
  os << std::setw(2) << output;
}

std::ostream& operator<<(std::ostream& os, ProgramNode const& prog) {
  print_prog(os, prog);
  return os;
}
}  // namespace flang
