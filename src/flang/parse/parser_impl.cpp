#include "parser_impl.hpp"

#include <memory>
#include <vector>

#include "flang/flang_exception.hpp"
#include "flang/parse/ast.hpp"
#include "flang/tokenize/token_type.hpp"

namespace flang
{
bool ParserImpl::atEOF() const
{
    return next_index_ >= tokens_.size();
}

std::optional<Token> ParserImpl::peekOptional() const
{
    return (atEOF() ? std::optional<Token>() : tokens_[next_index_]);
}

Token ParserImpl::peekNext() const
{
    auto token = peekOptional();
    if (!token.has_value()) {
        throw parser_exception("Unexpected EOF");
    }
    return token.value();
}

std::optional<Token> ParserImpl::takeOptional()
{
    return (atEOF() ? std::optional<Token>() : tokens_[next_index_++]);
}

Token ParserImpl::takeNext()
{
    auto token = takeOptional();
    if (!token.has_value()) {
        throw parser_exception("Unexpected EOF");
    }
    return token.value();
}

Program ParserImpl::parseProgram()
{
    std::vector<std::shared_ptr<Element>> elements;
    while (!atEOF()) {
        elements.emplace_back(parseElement());
    }

    return elements;
}

Token ParserImpl::eat(TokenType token_type)
{
    auto token = takeNext();
    if (token.type() != token_type) {
        throw parser_exception(std::string("Unexpected token (eat): ") + token.value());
    }
    return token;
}

// =====        Element Parsers         =====
// ==========================================

std::shared_ptr<Element> ParserImpl::parseElement()
{
    auto token = peekNext();
    switch (token.type()) {
        case tkIDENTIFIER:
            return parseIdentifier();
        case tkLPAREN:
            return parseList();
        case tkQUOTEMARK:
            return parseQuotedElement();
        default:
            return parseLiteral();
    }
}

std::shared_ptr<Identifier> ParserImpl::parseIdentifier()
{
    auto token_value = eat(tkIDENTIFIER).value();
    return std::make_shared<Identifier>(token_value);
}

std::shared_ptr<List> ParserImpl::parseList()
{
    eat(tkLPAREN);

    std::vector<std::shared_ptr<Element>> elements;
    for (auto tok = peekNext(); tok.type() != tkRPAREN; tok = peekNext()) {
        elements.emplace_back(parseElement());
    }

    eat(tkRPAREN);

    return std::make_shared<List>(std::move(elements));
}

std::shared_ptr<List> ParserImpl::parseQuotedElement()
{
    eat(tkQUOTEMARK);
    auto element    = parseElement();
    auto quote_node = std::make_shared<Identifier>("quote");
    return std::make_shared<List>(std::vector<std::shared_ptr<Element>>({quote_node, element}));
}

// =====        Literal Parsers         =====
// ==========================================


std::shared_ptr<Literal> ParserImpl::parseLiteral()
{
    auto token = peekNext();
    switch (token.type()) {
        case tkINTEGER:
            return parseInteger();
        case tkREAL:
            return parseReal();
        case tkBOOLEAN:
            return parseBoolean();
        case tkNULL:
            return parseNull();
        default:
            throw std::runtime_error("Unexpected token parse literal");
    }
}

std::shared_ptr<Integer> ParserImpl::parseInteger()
{
    auto token_value = eat(tkINTEGER).value();
    return std::make_shared<Integer>(std::stoi(token_value));
}

std::shared_ptr<Real> ParserImpl::parseReal()
{
    auto token_value = eat(tkREAL).value();
    return std::make_shared<Real>(std::stod(token_value));
}

std::shared_ptr<Boolean> ParserImpl::parseBoolean()
{
    auto token_value = eat(tkBOOLEAN).value();
    return std::make_shared<Boolean>(token_value == "true" ? true : false);
}

std::shared_ptr<Null> ParserImpl::parseNull()
{
    eat(tkNULL);
    return std::make_shared<Null>();
}

} // namespace flang
