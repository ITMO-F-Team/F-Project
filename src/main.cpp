#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./main <source_file>";
    return 1;
  }
  std::string source_file_name = argv[1];

  std::cout << "Interpreter is not implemented yet...\n";
  return 0;
}