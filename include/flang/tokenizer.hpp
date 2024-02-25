#pragma once

#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "token.hpp"

namespace flang {
class Tokenizer {
 public:
  std::vector<Token> tokenize(std::string const& source) const;

 private:
  std::string remove_comments(std::string const& source) const;

  // clang-format off
  const std::unordered_map<TokenType, std::regex> token_to_regex_ = {
      {tkLPAREN, std::regex("\\(")},
      {tkRPAREN, std::regex("\\)")},
      {tkQUOTE, std::regex("quote")},
      {tkQUOTEMARK, std::regex("'")},
      {tkSETQ, std::regex("setq")},
      {tkFUNC, std::regex("func")},
      {tkLAMBDA, std::regex("lambda")},
      {tkPROG, std::regex("prog")},
      {tkCOND, std::regex("cond")},
      {tkWHILE, std::regex("while")},
      {tkRETURN, std::regex("return")},
      {tkBREAK, std::regex("break")},
      {tkIDENTIFIER, std::regex("[a-zA-Z][a-zA-Z0-9]*")},
      {tkINTEGER, std::regex("[+-]?\\d+")},
      {tkREAL, std::regex("[+-]?\\d+\\.\\d+")}
  };
  // clang-format on
};
}  // namespace flang
