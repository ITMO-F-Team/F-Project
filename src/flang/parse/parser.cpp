#include <algorithm>
#include <flang/flang_exception.hpp>
#include <flang/parse/ast.hpp>
#include <flang/pp.hpp>
#include <flang/tokenize/token.hpp>
#include <flang/tokenize/token_type.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace flang {

class Parser {
public:
  Parser(std::vector<Token> &tokens) : tokens(tokens), next_index(0) {}

  std::unique_ptr<ProgramNode> parse_program() {
    std::vector<std::unique_ptr<ElementNode>> elements;

    while (!at_eof()) {
      auto el = parse_element();
      elements.push_back(std::move(el));
    }

    auto node = new ProgramNode(std::move(elements));
    return std::unique_ptr<ProgramNode>{node};
  }

private:
  std::vector<Token> &tokens;
  int next_index;

  // ----- Parsers for main non-terminals -----

  /**
  Element :=
    | Identifier
    | IntegerLiteral
    | RealLiteral
    | ListLikeElement
  */
  std::unique_ptr<ElementNode> parse_element() {
    auto tok = peek1();
    auto tok_type = tok.type();
    if (tok_type == tkLPAREN) {
      return parse_listlike_element();
    } else if (tok_type == tkIDENTIFIER) {
      return parse_identifier();
    } else if (tok_type == tkINTEGER) {
      return parse_integer_literal();
    } else if (tok_type == tkREAL) {
      return parse_real_literal();
    } else {
      // TODO: 🆘 please add the token's value in the error message
      throw parser_exception("Unexpected 'TOKEN' when parsing an element");
    }
  }

  /**
  Identifier := tkIDENTIFIER.
  */
  std::unique_ptr<IdentifierNode> parse_identifier() {
    auto token = eat(tkIDENTIFIER);
    auto node = new IdentifierNode(token.value());
    return std::unique_ptr<IdentifierNode>{node};
  }

  /**
  IntegerLiteral := tkINTEGER.
  */
  std::unique_ptr<IntegerLiteralNode> parse_integer_literal() {
    auto token = eat(tkINTEGER);
    auto node = new IntegerLiteralNode(std::stoi(token.value()));
    return std::unique_ptr<IntegerLiteralNode>(node);
  }

  /**
  RealLiteral := tkREAL.
  */
  std::unique_ptr<RealLiteralNode> parse_real_literal() {
    auto token = eat(tkREAL);
    // TODO: for some reason reals are converted to int
    auto node = new RealLiteralNode(std::stod(token.value()));
    return std::unique_ptr<RealLiteralNode>(node);
  }

  /**
  PureList := ( Element* ).
  TODO: maybe remove?
   */
  std::unique_ptr<PureListNode> parse_pure_list() {
    std::vector<std::unique_ptr<ElementNode>> elements;

    eat(tkLPAREN);
    auto next_token = peek1();
    while (next_token.type() != tkRPAREN) {
      auto element = parse_element();
      elements.push_back(std::move(element));
      next_token = peek1();
    }
    eat(tkRPAREN);

    auto node = new PureListNode(std::move(elements));
    return std::unique_ptr<PureListNode>(node);
  }

  /**
  QuoteShortForm := ' Element.
  */
  std::unique_ptr<QuoteNode> parse_quote_short_form() {
    eat(tkQUOTEMARK);
    auto arg = parse_element();
    auto node = new QuoteNode(std::move(arg));
    return std::unique_ptr<QuoteNode>(node);
  }

  // --- List-like elements parsers ---
  // i.e. parsers for quote, setq, func, lambda ...
  //
  // WARNING: Special parsers (except for the `parse_special_element`) must not:
  //  1. eat the starting left parenthesis
  //  2. eat the ending right parenthesis

  /**
    ListLikeElement := ( ListLikeElementBody ).
    ListLikeElementBody :=
      | Call
      | Quote
      | Setq
      | Func
      | Lambda
      | Prog
      | Cond
      | While
      | Return
      | Break
  */
  std::unique_ptr<ElementNode> parse_listlike_element() {
    std::unique_ptr<ElementNode> result;

    eat(tkLPAREN);
    auto tok_type = peek1().type();
    if (tok_type == tkQUOTE) {
      result = parse_listlike_quote();
    } else if (tok_type == tkFUNC) {
      result = parse_listlike_func();
    } else if (tok_type == tkIDENTIFIER) {
      result = parse_listlike_call();
    } else {
      throw parser_exception("Unexpected token");
    }
    eat(tkRPAREN);

    return result;
  }

  /**
    Call := Identifier Identifier*
  */
  std::unique_ptr<CallNode> parse_listlike_call() {
    std::vector<std::unique_ptr<ElementNode>> args;

    auto callee = eat(tkIDENTIFIER);

    auto next_token = peek1();
    while (next_token.type() != tkRPAREN) {
      auto arg_element = parse_element();
      args.push_back(std::move(arg_element));
      next_token = peek1();
    }

    // TODO: 🆘 please rewrite using make_unique or whatever
    auto node = new CallNode(
        std::make_unique<IdentifierNode>(IdentifierNode(callee.value())),
        std::move(args));
    return std::unique_ptr<CallNode>{node};
  }

  /**
  Quote := quote Element.
  */
  std::unique_ptr<QuoteNode> parse_listlike_quote() {
    eat(tkQUOTE);
    auto arg = parse_element();

    auto node = new QuoteNode(std::move(arg));
    return std::unique_ptr<QuoteNode>(node);
  }

  /**
  Func := func Identifier ( Identifier* ) Element.
  */
  std::unique_ptr<FuncNode> parse_listlike_func() {
    eat(tkFUNC);
    auto ident = parse_identifier();
    auto args = parse_func_args();
    auto body = parse_element();

    auto node =
        new FuncNode(std::move(ident), std::move(args), std::move(body));
    return std::unique_ptr<FuncNode>(node);
  }

  std::vector<std::unique_ptr<IdentifierNode>> parse_func_args() {
    std::vector<std::unique_ptr<IdentifierNode>> result;

    eat(tkLPAREN);
    auto tok = peek1();
    auto tok_type = tok.type();
    while (tok_type != tkRPAREN) {
      auto ident = parse_identifier();
      result.push_back(std::move(ident));
      tok = peek1();
    }
    eat(tkRPAREN);

    return result;
  }

  // ----- Primitive parsers -----

  Token eat(TokenType token_type) {
    auto token = take1();

    if (token.type() == token_type) {
      return token;
    } else {
      std::cout << "Unexpected token: " << token << "\n";
      throw parser_exception("Unexpected token");
    }
  }

  std::optional<Token> take() {
    if (!at_eof()) {
      return tokens[next_index++];
    } else {
      return {};
    }
  }

  Token take1() {
    auto t = take();
    if (!t.has_value()) {
      throw parser_exception("Unexpected EOF");
    }
    return t.value();
  }

  std::optional<Token> peek() {
    if (!at_eof()) {
      return tokens[next_index];
    } else {
      return {};
    }
  }

  Token peek1() {
    auto t = peek();
    if (!t.has_value()) {
      throw parser_exception("Unexpected EOF");
    }
    return t.value();
  }

  bool at_eof() { return next_index >= tokens.size(); }
};

std::unique_ptr<ProgramNode> parse(std::vector<Token> &tokens) {
  auto parser = new Parser(tokens);
  return parser->parse_program();
}
} // namespace flang
