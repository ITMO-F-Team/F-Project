#include <flang/parse/ast.hpp>
#include <stdexcept>
#include <string>

namespace flang {

void shit_out_identifier(std::ostream& os, IdentifierNode const& node);

void shit_out_call_node(std::ostream& os, CallNode const& node);

void shit_out_integer_literal(std::ostream& os, IntegerLiteralNode const& node);

void shit_out_real_literal(std::ostream& os, RealLiteralNode const& node);

// 🤡 Эти ебаные перегрузки (<<) не выбираются так как я хочу
// Поэтому сделаю по-старинке
void shit_out_element(std::ostream& os, ElementNode const& element) {
  const IdentifierNode* identifier_node = dynamic_cast<IdentifierNode const*>(&element);
  if (identifier_node) {
    shit_out_identifier(os, *identifier_node);
  } else {
    const CallNode* call_node = dynamic_cast<CallNode const*>(&element);
    if (call_node) {
      shit_out_call_node(os, *call_node);
    } else {
      const IntegerLiteralNode* integer_node = dynamic_cast<IntegerLiteralNode const*>(&element);
      if (integer_node) {
        shit_out_integer_literal(os, *integer_node);
      } else {
        auto node = dynamic_cast<RealLiteralNode const*>(&element);
        if (node) {
          shit_out_real_literal(os, *node);
        } else {
          throw std::runtime_error("Not implemented");
        }
      }
    }
  }
}

void shit_out_identifier(std::ostream& os, IdentifierNode const& node) { os << node.name(); }

void shit_out_integer_literal(std::ostream& os, IntegerLiteralNode const& node) { os << std::to_string(node.value()); }

void shit_out_real_literal(std::ostream& os, RealLiteralNode const& node) { os << std::to_string(node.value()); }

void shit_out_call_node(std::ostream& os, CallNode const& node) {
  os << std::string("(Call ");
  shit_out_identifier(os, node.callee());
  os << std::string(" [");
  for (auto& arg : node.args()) {
    shit_out_element(os, *arg.get());
    os << std::string(", ");
  }
  os << std::string("])");
}

void shit_out_prog(std::ostream& os, ProgramNode const& node) {
  os << std::string("(Program \n");
  for (auto& el : node.elements()) {
    shit_out_element(os, *el.get());
    os << std::string("\n");
  }
  os << std::string(")");
}

std::ostream& operator<<(std::ostream& os, ElementNode const& element) {
  shit_out_element(os, element);
  return os;
}

std::ostream& operator<<(std::ostream& os, IdentifierNode const& node) {
  shit_out_identifier(os, node);
  return os;
}

std::ostream& operator<<(std::ostream& os, CallNode const& node) {
  shit_out_call_node(os, node);
  return os;
}

std::ostream& operator<<(std::ostream& os, ProgramNode const& prog) {
  shit_out_prog(os, prog);
  return os;
}
}  // namespace flang
