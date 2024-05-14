#pragma once

#include <string>

#include "token_type.hpp"

namespace flang
{
class Token
{
public:
    struct TokenLocation {
        size_t line;
        size_t col;
    };

    Token(TokenType type, std::string value, size_t line, size_t col);

    std::string const& value() const;

    std::string value();

    TokenType type() const;

    TokenLocation location() const;

private:
    TokenType type_;
    std::string value_;
    TokenLocation location_;
};
} // namespace flang
