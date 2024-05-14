#include "flang/tokenize/tokenizer.hpp"
#include "flang/flang_exception.hpp"
#include <sstream>

namespace flang
{
std::vector<Token> Tokenizer::tokenize(std::string const& source) const
{
    std::vector<Token> tokenized_source;

    std::istringstream source_stream(source);
    std::string line;
    size_t line_number = 0;

    while (std::getline(source_stream, line)) {
        auto from_it = line.cbegin();
        while (from_it != line.end()) {
            if (std::isspace(*from_it)) {
                ++from_it;
                continue;
            }
            std::smatch match;
            for (auto&& [token_type, pattern] : token_to_regex_) {
                if (std::regex_search(from_it, line.cend(), match, pattern, std::regex_constants::match_continuous)) {
                    size_t col_number = std::distance(line.cbegin(), from_it);
                    tokenized_source.emplace_back(token_type, match.str(), line_number, col_number);
                    from_it += match.length();
                    break;
                }
            }
            if (match.empty()) {
                throw tokenizer_exception("Unexpected symbol met: " + std::string(1, *from_it));
            }
        }
        ++line_number;
    }

    return tokenized_source;
}
} // namespace flang
