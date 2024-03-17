#pragma once

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

  // ----- Parsers for main non-terminals -----
  /**
  Element :=
    | Identifier
    | IntegerLiteral
    | RealLiteral
    | ListLikeElement
  */
  std::unique_ptr<ElementNode> parseElement();

  /**
  Identifier := tkIDENTIFIER.
  */
  std::unique_ptr<IdentifierNode> parseIdentifier();

  /**
  IntegerLiteral := tkINTEGER.
  */
  std::unique_ptr<IntegerLiteralNode> parseIntegerLiteral();

  /**
  RealLiteral := tkREAL.
  */
  std::unique_ptr<RealLiteralNode> parseRealLiteral();

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
  std::unique_ptr<ElementNode> parseListLikeElement();

  std::unique_ptr<PureListNode> parseList();

  std::unique_ptr<CallNode> parseListLikeCall();

  std::unique_ptr<QuoteNode> parseListLikeQuote();

  std::unique_ptr<SetqNode> parseListLikeSetq();

  std::unique_ptr<FuncNode> parseListLikeFunc();

  std::unique_ptr<LambdaNode> parseListLikeLambda();

  std::unique_ptr<ProgNode> parseListLikeProg();

  std::unique_ptr<CondNode> parseListLikeCond();

  std::unique_ptr<WhileNode> parseListLikeWhile();

  std::unique_ptr<ReturnNode> parseListLikeReturn();

  std::unique_ptr<BreakNode> parseListLikeBreak();

  std::vector<std::unique_ptr<IdentifierNode>> parseFuncArguments();
};
}  // namespace flang
