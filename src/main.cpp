#include <flang/parse/ast.hpp>
#include <flang/parse/parser.hpp>
#include <fstream>
#include <iostream>
#include <iterator>

#include "flang/eval/eval_visitor.hpp"
#include "flang/flang_exception.hpp"
#include "flang/tokenize/tokenizer.hpp"


std::string readSource(std::string const& source_file_name)
{
    std::ifstream input(source_file_name);
    if (!input.is_open()) {
        throw std::runtime_error("Couldn't open file " + source_file_name);
    }
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage: ./main <source_file>";
        return 1;
    }
    std::string source_file_name = argv[1];
    std::string source           = readSource(source_file_name);
    try {
        auto tokens = flang::Tokenizer().tokenize(source);
        auto prog   = flang::parse(tokens);
        flang::EvalVisitor().visitProgram(prog);
    } catch (flang::flang_exception const& e) {
        std::cerr << "\nERROR: " << e.what();
        return 1;
    }
    return 0;
}
