#pragma once

#include <optional>
#include <vector>

#include "flang/parse/ast.hpp"
#include "flang/tokenize/token.hpp"

namespace flang {
class ParserImpl {
 public:
  explicit ParserImpl(std::vector<Token> const& tokens) : tokens_(tokens), next_index_(0) {}

  std::shared_ptr<ProgramNode> parseProgram();

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

  // ----- Parsers for main non-terminals -----
  /**
  Element :=
    | Identifier
    | IntegerLiteral
    | RealLiteral
    | ShortQuote
    | ListLikeElement
  */
  std::shared_ptr<ElementNode> parseElement();

  /**
  Identifier := tkIDENTIFIER.
  */
  std::shared_ptr<IdentifierNode> parseIdentifier();

  /**
  IntegerLiteral := tkINTEGER.
  */
  std::shared_ptr<IntegerLiteralNode> parseIntegerLiteral();

  /**
  RealLiteral := tkREAL.
  */
  std::shared_ptr<RealLiteralNode> parseRealLiteral();

  /**
  ShortQuote := ' Element.
  */
  std::shared_ptr<QuoteNode> parseShortQuote();

  /**
  ListLikeElement := ( ListLikeElementBody ).
  ListLikeElementBody :=
    | Call
    | List
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
  std::shared_ptr<ElementNode> parseListLikeElement();

  /**
  List := Element* .
  */
  std::shared_ptr<PureListNode> parseList();

  /**
  Quote := quote Element .
  */
  std::shared_ptr<QuoteNode> parseListLikeQuote();

  /**
  Setq := setq Ident Element .
  */
  std::shared_ptr<SetqNode> parseListLikeSetq();

  std::shared_ptr<FuncNode> parseListLikeFunc();

  std::shared_ptr<LambdaNode> parseListLikeLambda();

  std::shared_ptr<ProgNode> parseListLikeProg();

  std::shared_ptr<CondNode> parseListLikeCond();

  std::shared_ptr<WhileNode> parseListLikeWhile();

  std::shared_ptr<ReturnNode> parseListLikeReturn();

  std::shared_ptr<BreakNode> parseListLikeBreak();

  std::vector<std::shared_ptr<IdentifierNode>> parseFuncArguments();
};
}  // namespace flang
