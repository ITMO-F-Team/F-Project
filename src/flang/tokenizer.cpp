#include "flang/tokenizer.hpp"

#include "flang/flang_exception.hpp"

namespace flang {
std::string Tokenizer::remove_comments(std::string const& source) const {
  std::istringstream iss(source);
  std::ostringstream oss;
  std::string line;

  while (std::getline(iss, line)) {
    line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) { return !std::isspace(ch); }));

    if (!line.empty() && line.find(";") != 0) {
      oss << line << '\n';
    }
  }

  return oss.str();
}

std::vector<Token> Tokenizer::tokenize(std::string const& source) const {
  std::vector<Token> tokenized_source;

  std::string const& cleaned_source = remove_comments(source);

  auto from_it = cleaned_source.begin();
  while (from_it != cleaned_source.end()) {
    if (std::isspace(*from_it)) {
      ++from_it;
      continue;
    }
    std::smatch match;
    for (auto&& [token_type, pattern] : token_to_regex_) {
      if (std::regex_search(from_it, cleaned_source.end(), match, pattern, std::regex_constants::match_continuous)) {
        tokenized_source.emplace_back(token_type, match.str());
        from_it += match.length();
        break;
      }
    }
    if (match.empty()) {
      throw tokenizer_exception("Unexpected symbol met: " + std::string(1, *from_it));
    }
  }
  return tokenized_source;
}
}  // namespace flang
