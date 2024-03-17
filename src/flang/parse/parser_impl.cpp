#include "parser_impl.hpp"

#include <iostream>

#include "flang/flang_exception.hpp"
#include "flang/pp/print_token.hpp"

namespace flang {
bool ParserImpl::atEOF() const { return next_index_ >= tokens_.size(); }

std::optional<Token> ParserImpl::peekOptional() const {
  return (atEOF() ? std::optional<Token>() : tokens_[next_index_]);
}

Token ParserImpl::peekNext() const {
  auto token = peekOptional();
  if (!token.has_value()) {
    throw parser_exception("Unexpected EOF");
  }
  return token.value();
}

std::optional<Token> ParserImpl::takeOptional() { return (atEOF() ? std::optional<Token>() : tokens_[next_index_++]); }

Token ParserImpl::takeNext() {
  auto token = takeOptional();
  if (!token.has_value()) {
    throw parser_exception("Unexpected EOF");
  }
  return token.value();
}

std::unique_ptr<ProgramNode> ParserImpl::parseProgram() {
  std::vector<std::unique_ptr<ElementNode>> elements;
  while (!atEOF()) {
    elements.emplace_back(parseElement());
  }

  return std::make_unique<ProgramNode>(std::move(elements));
}

Token ParserImpl::eat(TokenType token_type) {
  auto token = takeNext();
  if (token.type() != token_type) {
    throw parser_exception(std::string("Unexpected token (eat): ") + token.value());
  }
  return token;
}

std::unique_ptr<ElementNode> ParserImpl::parseElement() {
  auto token = peekNext();
  switch (token.type()) {
    case tkLPAREN:
      return parseListLikeElement();
    case tkIDENTIFIER:
      return parseIdentifier();
    case tkINTEGER:
      return parseIntegerLiteral();
    case tkREAL:
      return parseRealLiteral();
    default:
      throw parser_exception(std::string("Unexpected token (parseElement): ") + token.value());
  }
}

std::unique_ptr<ElementNode> ParserImpl::parseListLikeElement() {
  std::unique_ptr<ElementNode> list_like_node;

  eat(tkLPAREN);
  auto token = peekNext();
  switch (token.type()) {
    case tkQUOTE:
      list_like_node = parseListLikeQuote();
      break;
    case tkSETQ:
      list_like_node = parseListLikeSetq();
      break;
    case tkIDENTIFIER:
      list_like_node = parseListLikeCall();
      break;
    case tkWHILE:
      list_like_node = parseListLikeWhile();
      break;
    case tkRETURN:
      list_like_node = parseListLikeReturn();
      break;
    case tkBREAK:
      list_like_node = parseListLikeBreak();
      break;
    default:
      throw parser_exception(std::string("Unexpected token (list): ") + token.value());
  }
  eat(tkRPAREN);

  return list_like_node;
}

std::unique_ptr<QuoteNode> ParserImpl::parseListLikeQuote() {
  eat(tkQUOTE);
  return std::make_unique<QuoteNode>(parseElement());
}

std::unique_ptr<CallNode> ParserImpl::parseListLikeCall() {
  std::vector<std::unique_ptr<ElementNode>> args;

  auto callee = eat(tkIDENTIFIER);

  for (auto next_token = peekNext(); next_token.type() != tkRPAREN; next_token = peekNext()) {
    args.emplace_back(parseElement());
  }
  return std::make_unique<CallNode>(std::make_unique<IdentifierNode>(callee.value()), std::move(args));
}

std::unique_ptr<SetqNode> ParserImpl::parseListLikeSetq() {
  eat(tkSETQ);
  auto name = parseIdentifier();
  auto value = parseElement();
  return std::make_unique<SetqNode>(std::move(name), std::move(value));
}

std::unique_ptr<WhileNode> ParserImpl::parseListLikeWhile() {
  eat(tkWHILE);
  auto condition = parseElement();
  auto body = parseElement();
  return std::make_unique<WhileNode>(std::move(condition), std::move(body));
}

std::unique_ptr<ReturnNode> ParserImpl::parseListLikeReturn() {
  eat(tkRETURN);
  return std::make_unique<ReturnNode>(parseElement());
}

std::unique_ptr<BreakNode> ParserImpl::parseListLikeBreak() {
  eat(tkBREAK);
  return std::make_unique<BreakNode>();
}

std::unique_ptr<IdentifierNode> ParserImpl::parseIdentifier() {
  auto token_value = eat(tkIDENTIFIER).value();
  return std::make_unique<IdentifierNode>(token_value);
}

std::unique_ptr<IntegerLiteralNode> ParserImpl::parseIntegerLiteral() {
  auto token_value = eat(tkINTEGER).value();
  return std::make_unique<IntegerLiteralNode>(std::stoi(token_value));
}

std::unique_ptr<RealLiteralNode> ParserImpl::parseRealLiteral() {
  auto token_value = eat(tkREAL).value();
  return std::make_unique<RealLiteralNode>(std::stod(token_value));
}
}  // namespace flang
