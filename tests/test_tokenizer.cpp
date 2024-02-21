#include <gtest/gtest.h>

#include "flang/tokenizer.hpp"

using namespace flang;

std::vector<TokenType> getTypesVector(std::vector<Token> const& tokens) {
  std::vector<TokenType> types;
  std::transform(tokens.begin(), tokens.end(), std::back_inserter(types), [](auto&& token) { return token.type(); });
  return types;
}

TEST(TokenizeString, SingleToken) {
  std::string source = "break";
  Tokenizer tokenizer;
  auto tokens = tokenizer.tokenize(source);
  ASSERT_EQ(getTypesVector(tokens), std::vector<TokenType>{tkBREAK});
}

TEST(TokenizeString, MultipleTokens) {
  std::string source = "(cond (lesseq n 1) )";
  Tokenizer tokenizer;
  auto tokens = tokenizer.tokenize(source);
  std::vector<TokenType> actual = {tkLPAREN,     tkCOND,    tkLPAREN, tkIDENTIFIER,
                                   tkIDENTIFIER, tkINTEGER, tkRPAREN, tkRPAREN};
  ASSERT_EQ(getTypesVector(tokens), actual);
}
