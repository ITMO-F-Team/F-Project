#include <flang/parse/ast.hpp>
#include <string>
#include <utility>
#include <vector>
namespace flang
{

struct Builtin {
    std::string name;
    std::function<void(Visitor&, std::vector<std::shared_ptr<Element>>)> impl;
};

std::vector<Builtin> getBuiltins();

} // namespace flang
