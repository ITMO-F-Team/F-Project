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
    std::cout << "Unexpected token: " << token << '\n';
    throw parser_exception("Unexpected token");
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
  }
  throw parser_exception(std::string("Unexpected token: ") + token.value());
}

std::unique_ptr<ElementNode> ParserImpl::parseListLikeElement() { return {}; }

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
