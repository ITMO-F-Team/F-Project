#include "flang/tokenizer.hpp"

#include <stdexcept>

namespace flang {
std::vector<Token> Tokenizer::tokenize(std::string const& source) const {
  std::vector<Token> tokenized_source;

  auto from_it = source.begin();
  while (from_it != source.end()) {
    if (*from_it == ' ') {
      ++from_it;
      continue;
    }
    std::smatch match;
    for (auto&& [token_type, pattern] : token_to_regex_) {
      if (std::regex_search(from_it, source.end(), match, pattern, std::regex_constants::match_continuous)) {
        tokenized_source.emplace_back(token_type, match.str());
        from_it += match.length();
        break;
      }
    }
    if (match.empty()) {
      // TODO: Descriptive error msg
      throw std::runtime_error("Unexpected symbol met");
    }
  }
  return tokenized_source;
}
}  // namespace flang