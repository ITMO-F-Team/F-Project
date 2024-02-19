#pragma once

#include <regex>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "token.hpp"

namespace flang {
class Tokenizer {
 public:
  std::vector<Token> tokenize(std::string_view source) const;

 private:
  // clang-format off
  const std::unordered_map<TokenType, std::regex> token_to_regex = {
      {tkLPAREN, std::regex("(")},
      {tkRPAREN, std::regex(")")},
      {tkQUOTE, std::regex("quote")},
      {tkSETQ, std::regex("setq")},
      {tkFUNC, std::regex("func")},
      {tkLAMBDA, std::regex("lambda")},
      {tkPROG, std::regex("prog")},
      {tkCOND, std::regex("cond")},
      {tkWHILE, std::regex("while")},
      {tkRETURN, std::regex("return")},
      {tkBREAK, std::regex("break")},
      {tkNULL, std::regex("null")},
      {tkIDENTIFIER, std::regex(R"(\b[a-zA-Z][a-zA-Z0-9]*\b)")}
  };
  // clang-format on
};
}  // namespace flang