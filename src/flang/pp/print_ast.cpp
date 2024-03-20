#include <flang/parse/ast.hpp>
#include <stdexcept>
#include <string>

namespace flang {

void print_identifier(std::ostream& os, IdentifierNode const& node);

void print_call_node(std::ostream& os, CallNode const& node);

void print_integer_literal(std::ostream& os, IntegerLiteralNode const& node);

void print_real_literal(std::ostream& os, RealLiteralNode const& node);

void print_setq(std::ostream& os, SetqNode const& node);

void print_cond(std::ostream& os, CondNode const& node);

void print_while(std::ostream& os, WhileNode const& node);

void print_pure_list(std::ostream& os, PureListNode const& node);

void print_prog(std::ostream& os, ProgNode const& node);

void print_func(std::ostream& os, FuncNode const& node);

void print_return(std::ostream& os, ReturnNode const& node);

void print_lambda(std::ostream& os, LambdaNode const& node);

void print_quote(std::ostream& os, QuoteNode const& node);

void print_break(std::ostream& os, BreakNode const& node);

void print_element(std::ostream& os, ElementNode const& element) {
  switch (element.type()) {
    case ElementNodeType::Identifier:
      if (const IdentifierNode* identifier_node = dynamic_cast<IdentifierNode const*>(&element)) {
        print_identifier(os, *identifier_node);
      }
      break;
    case ElementNodeType::Call:
      if (const CallNode* call_node = dynamic_cast<CallNode const*>(&element)) {
        print_call_node(os, *call_node);
      }
      break;
    case ElementNodeType::IntegerLiteral:
      if (const IntegerLiteralNode* integer_node = dynamic_cast<IntegerLiteralNode const*>(&element)) {
        print_integer_literal(os, *integer_node);
      }
      break;
    case ElementNodeType::RealLiteral:
      if (const RealLiteralNode* real_node = dynamic_cast<RealLiteralNode const*>(&element)) {
        print_real_literal(os, *real_node);
      }
      break;
    case ElementNodeType::Setq:
      if (const SetqNode* setq_node = dynamic_cast<SetqNode const*>(&element)) {
        print_setq(os, *setq_node);
      }
      break;
    case ElementNodeType::Cond:
      if (const CondNode* cond_node = dynamic_cast<CondNode const*>(&element)) {
        print_cond(os, *cond_node);
      }
      break;
    case ElementNodeType::While:
      if (const WhileNode* while_node = dynamic_cast<WhileNode const*>(&element)) {
        print_while(os, *while_node);
      }
      break;
    case ElementNodeType::PureList:
      if (const PureListNode* pure_list_node = dynamic_cast<PureListNode const*>(&element)) {
        print_pure_list(os, *pure_list_node);
      }
      break;
    case ElementNodeType::Prog:
      if (const ProgNode* prog_node = dynamic_cast<ProgNode const*>(&element)) {
        print_prog(os, *prog_node);
      }
      break;
    case ElementNodeType::Func:
      if (const FuncNode* func_node = dynamic_cast<FuncNode const*>(&element)) {
        print_func(os, *func_node);
      }
      break;
    case ElementNodeType::Return:
      if (const ReturnNode* return_node = dynamic_cast<ReturnNode const*>(&element)) {
        print_return(os, *return_node);
      }
      break;
    case ElementNodeType::Lambda:
      if (const LambdaNode* lambda_node = dynamic_cast<LambdaNode const*>(&element)) {
        print_lambda(os, *lambda_node);
      }
      break;
    case ElementNodeType::Quote:
      if (const QuoteNode* quote_node = dynamic_cast<QuoteNode const*>(&element)) {
        print_quote(os, *quote_node);
      }
      break;
    case ElementNodeType::Break:
      if (const BreakNode* break_node = dynamic_cast<BreakNode const*>(&element)) {
        print_break(os, *break_node);
      }
      break;
    default:
      throw std::runtime_error("Not implemented");
  }
}

void print_break(std::ostream& os, BreakNode const& node) { os << std::string("(Break)"); }

void print_quote(std::ostream& os, QuoteNode const& node) {
  os << std::string("(Quote {");
  print_element(os, node.arg());
  os << std::string("})");
}

void print_lambda(std::ostream& os, LambdaNode const& node) {
  os << std::string("(Lambda {");
  os << std::string(",\t\nArguments: ");
  for (const auto& arg : node.args()) {
    print_element(os, *arg);
    os << std::string(", ");
  }
  os << std::string(",\t\nBody: ");
  print_element(os, node.body());
  os << std::string("})");
}

void print_return(std::ostream& os, ReturnNode const& node) {
  os << std::string("(Return {");
  os << std::string("Value: ");
  print_element(os, node.value());
  os << std::string("})");
}

void print_func(std::ostream& os, FuncNode const& node) {
  os << std::string("(Func {");
  os << std::string("\t\nIdentifier: ");
  print_element(os, node.name());
  os << std::string(",\t\nArguments: ");
  for (const auto& arg : node.args()) {
    print_element(os, *arg);
    os << std::string(", ");
  }
  os << std::string(",\t\nBody: ");
  print_element(os, node.body());
  os << std::string("})");
}

void print_prog(std::ostream& os, ProgNode const& node) {
  os << std::string("(Prog {");
  os << std::string("Arguments: [");
  for (const auto& arg : node.args()) {
    print_element(os, *arg);
    os << std::string(", ");
  }
  os << std::string("]");
  os << std::string(", Body: ");
  print_element(os, node.body());
  os << std::string(")");
}

void print_pure_list(std::ostream& os, PureListNode const& node) {
  os << std::string("(PureListNode [");
  for (const auto& element : node.elements()) {
    os << std::string("\n\t");
    print_element(os, *element);
    os << std::string(",");
  }
  os << std::string("])");
}

void print_while(std::ostream& os, WhileNode const& node) {
  os << std::string("(While {");
  os << std::string("\nCondition: ");
  print_element(os, node.condition());
  os << std::string(",\nBody: [");
  print_element(os, node.body());
  os << std::string("]})");
}

void print_cond(std::ostream& os, CondNode const& node) {
  os << std::string("(Cond {");
  os << std::string("\nCondition: ");
  print_element(os, node.condition());
  os << std::string(",\nThen: ");
  print_element(os, node.thenBranch());
  os << std::string(",\nElse: ");
  if (node.elseBranch().has_value()) {
    print_element(os, node.elseBranch().value());
  } else {
    os << std::string("<empty>");
  }
  os << std::string("})");
}

void print_setq(std::ostream& os, SetqNode const& node) {
  os << std::string("(Setq {");
  os << std::string("Identifier: ");
  print_identifier(os, node.name());
  os << std::string(", Value: ");
  print_element(os, node.value());
  os << std::string("})");
}

void print_identifier(std::ostream& os, IdentifierNode const& node) { os << node.name(); }

void print_integer_literal(std::ostream& os, IntegerLiteralNode const& node) { os << std::to_string(node.value()); }

void print_real_literal(std::ostream& os, RealLiteralNode const& node) { os << std::to_string(node.value()); }

void print_call_node(std::ostream& os, CallNode const& node) {
  os << std::string("(Call ");
  print_identifier(os, node.callee());
  os << std::string(" [");
  for (auto& arg : node.args()) {
    print_element(os, *arg.get());
    os << std::string(", ");
  }
  os << std::string("])");
}

void print_prog(std::ostream& os, ProgramNode const& node) {
  os << std::string("(Program \n");
  for (auto& el : node.elements()) {
    print_element(os, *el.get());
    os << std::string("\n");
  }
  os << std::string(")");
}

std::ostream& operator<<(std::ostream& os, ElementNode const& element) {
  print_element(os, element);
  return os;
}

std::ostream& operator<<(std::ostream& os, IdentifierNode const& node) {
  print_identifier(os, node);
  return os;
}

std::ostream& operator<<(std::ostream& os, CallNode const& node) {
  print_call_node(os, node);
  return os;
}

std::ostream& operator<<(std::ostream& os, ProgramNode const& prog) {
  print_prog(os, prog);
  return os;
}
}  // namespace flang
