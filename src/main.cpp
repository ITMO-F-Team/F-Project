#include <algorithm>
#include <flang/eval/eval.hpp>
#include <flang/parse/ast.hpp>
#include <flang/parse/parser.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>

#include "flang/flang_exception.hpp"
#include "flang/pp.hpp"
#include "flang/tokenize/tokenizer.hpp"

std::string readSource(std::string const& source_file_name) {
  std::ifstream input(source_file_name);
  if (!input.is_open()) {
    throw std::runtime_error("Couldn't open file " + source_file_name);
  }
  return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./main <source_file>";
    return 1;
  }
  std::string source_file_name = argv[1];
  std::string source = readSource(source_file_name);

  std::cout << "Source program:\n" << source << '\n';
  try {
    auto tokens = flang::Tokenizer().tokenize(source);
    std::cout << "== Token representation ==\n";
    std::copy(tokens.begin(), tokens.end(), std::ostream_iterator<flang::Token>(std::cout, " "));
    std::cout << "\n\n";

    std::shared_ptr<flang::ProgramNode> prog = flang::parse(tokens);
    std::cout << "== AST ==\n";
    if (prog) {
      std::cout << *prog << '\n';
    }
    std::cout << "== EVAL ==\n";
    flang::eval(prog);
  } catch (flang::flang_exception const& e) {
    std::cerr << "\nERROR: " << e.what();
    return 1;
  }
  return 0;
}
