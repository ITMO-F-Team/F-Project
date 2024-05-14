#pragma once

#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "flang/tokenize/token_type.hpp"
#include "token.hpp"

namespace flang
{
class Tokenizer
{
public:
    std::vector<Token> tokenize(std::string const& source) const;

private:
    const std::vector<std::pair<TokenType, std::regex>> token_to_regex_ = {
        {tkLPAREN, std::regex("\\(")},
        {tkRPAREN, std::regex("\\)")},
        {tkQUOTEMARK, std::regex("'")},
        {tkIDENTIFIER, std::regex("[a-zA-Z][a-zA-Z0-9]*")},
        {tkINTEGER, std::regex("[+-]?\\d+")},
        {tkREAL, std::regex("[+-]?\\d+\\.\\d+")}};
};
} // namespace flang
