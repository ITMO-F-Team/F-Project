#include "parser_impl.hpp"

#include <flang/parse/ast.hpp>
#include <flang/tokenize/token_type.hpp>
#include <memory>
#include <vector>

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

std::shared_ptr<ProgramNode> ParserImpl::parseProgram() {
  std::vector<std::shared_ptr<ElementNode>> elements;
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

std::shared_ptr<ElementNode> ParserImpl::parseElement() {
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
    case tkQUOTEMARK:
      return parseShortQuote();
    default:
      throw parser_exception(std::string("Unexpected token (parseElement): ") + token.value());
  }
}

std::shared_ptr<QuoteNode> ParserImpl::parseShortQuote() {
  eat(tkQUOTEMARK);
  auto arg = parseElement();
  return std::make_unique<QuoteNode>(std::move(arg));
}

std::shared_ptr<ElementNode> ParserImpl::parseListLikeElement() {
  std::shared_ptr<ElementNode> list_like_node;

  eat(tkLPAREN);
  auto token = peekNext();
  switch (token.type()) {
    case tkQUOTE:
      list_like_node = parseListLikeQuote();
      break;
    case tkSETQ:
      list_like_node = parseListLikeSetq();
      break;
    case tkFUNC:
      list_like_node = parseListLikeFunc();
      break;
    case tkLAMBDA:
      list_like_node = parseListLikeLambda();
      break;
    case tkPROG:
      list_like_node = parseListLikeProg();
      break;
    case tkCOND:
      list_like_node = parseListLikeCond();
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
      list_like_node = parseList();
  }
  eat(tkRPAREN);

  return list_like_node;
}

std::shared_ptr<PureListNode> ParserImpl::parseList() {
  std::vector<std::shared_ptr<ElementNode>> elements;

  for (auto tok = peekNext(); tok.type() != tkRPAREN; tok = peekNext()) {
    elements.emplace_back(parseElement());
  }

  return std::make_unique<PureListNode>(std::move(elements));
}

std::shared_ptr<QuoteNode> ParserImpl::parseListLikeQuote() {
  eat(tkQUOTE);
  return std::make_unique<QuoteNode>(parseElement());
}

std::shared_ptr<SetqNode> ParserImpl::parseListLikeSetq() {
  eat(tkSETQ);
  auto name = parseIdentifier();
  auto value = parseElement();
  return std::make_unique<SetqNode>(std::move(name), std::move(value));
}

std::shared_ptr<CondNode> ParserImpl::parseListLikeCond() {
  eat(tkCOND);
  auto condition = parseElement();
  auto then_branch = parseElement();
  std::shared_ptr<ElementNode> else_branch = nullptr;
  if (peekNext().type() != tkRPAREN) {
    else_branch = parseElement();
  }
  return std::make_unique<CondNode>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::shared_ptr<WhileNode> ParserImpl::parseListLikeWhile() {
  eat(tkWHILE);
  auto condition = parseElement();
  auto body = parseElement();
  return std::make_unique<WhileNode>(std::move(condition), std::move(body));
}

std::shared_ptr<ReturnNode> ParserImpl::parseListLikeReturn() {
  eat(tkRETURN);
  return std::make_unique<ReturnNode>(parseElement());
}

std::shared_ptr<BreakNode> ParserImpl::parseListLikeBreak() {
  eat(tkBREAK);
  return std::make_unique<BreakNode>();
}

std::shared_ptr<IdentifierNode> ParserImpl::parseIdentifier() {
  auto token_value = eat(tkIDENTIFIER).value();
  return std::make_unique<IdentifierNode>(token_value);
}

std::shared_ptr<FuncNode> ParserImpl::parseListLikeFunc() {
  eat(tkFUNC);
  auto name = parseIdentifier();
  auto args = parseFuncArguments();
  auto body = parseElement();

  return std::make_unique<FuncNode>(std::move(name), std::move(args), std::move(body));
}

std::shared_ptr<LambdaNode> ParserImpl::parseListLikeLambda() {
  eat(tkLAMBDA);
  auto args = parseFuncArguments();
  auto body = parseElement();

  return std::make_shared<LambdaNode>(std::move(args), std::move(body));
}

std::shared_ptr<ProgNode> ParserImpl::parseListLikeProg() {
  eat(tkPROG);
  auto args = parseFuncArguments();
  auto body = parseElement();

  return std::make_unique<ProgNode>(std::move(args), std::move(body));
}

std::shared_ptr<IntegerLiteralNode> ParserImpl::parseIntegerLiteral() {
  auto token_value = eat(tkINTEGER).value();
  return std::make_unique<IntegerLiteralNode>(std::stoi(token_value));
}

std::shared_ptr<RealLiteralNode> ParserImpl::parseRealLiteral() {
  auto token_value = eat(tkREAL).value();
  return std::make_unique<RealLiteralNode>(std::stod(token_value));
}

std::vector<std::shared_ptr<IdentifierNode>> ParserImpl::parseFuncArguments() {
  std::vector<std::shared_ptr<IdentifierNode>> args;
  eat(tkLPAREN);
  for (auto arg_ident = peekNext(); arg_ident.type() != tkRPAREN; arg_ident = peekNext()) {
    args.emplace_back(parseIdentifier());
  }
  eat(tkRPAREN);
  return std::move(args);
}
}  // namespace flang
