#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "flang/parse/ast.hpp"
#include "flang/tokenize/token.hpp"

namespace flang {
class ParserImpl {
 public:
  explicit ParserImpl(std::vector<Token> const& tokens) : tokens_(tokens), next_index_(0) {}

  std::unique_ptr<ProgramNode> parseProgram();

 private:
  std::vector<Token> tokens_;
  size_t next_index_;

  // ----- Primitive parsers -----

  bool atEOF() const;

  std::optional<Token> peekOptional() const;

  Token peekNext() const;

  std::optional<Token> takeOptional();

  Token takeNext();

  Token eat(TokenType token_type);

  // =====        Element Parsers         =====
  // ==========================================

  /**
  Element :=
    | Identifier
    | List
    | QuotedElement  <--- is desugared into List
    | Literal
  */
  std::shared_ptr<Element> parseElement();

  /**
  Identifier := tkIDENTIFIER.
  */
  std::shared_ptr<Identifier> parseIdentifier();

  /**
  List := ( Element* ) .
  */
  std::unique_ptr<List> parseList();

  /**
  Quote := ' Element.
  */
  std::unique_ptr<QuoteNode> parseQuotedElement();

  // =====        Literal Parsers         =====
  // ==========================================

  /**
  Literal :=
    | Integer
    | Real
    | Boolean
    | Null
  */
  std::shared_ptr<Literal> parseLiteral();

  /**
  Integer := tkINTEGER.
  */
  std::shared_ptr<Integer> parseInteger();

  /**
  Real := tkREAL.
  */
  std::shared_ptr<Real> parseReal();

  /**
  Boolean := tkBOOLEAN .
  */
  std::shared_ptr<Boolean> parseBoolean();

  /**
  Null := tkNULL .
  */
  std::shared_ptr<CallNode> parseNull();
};
}  // namespace flang
