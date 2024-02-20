#pragma once

#include <iostream>
#include <vector>

#include "flang/token.hpp"

namespace flang {
std::ostream& operator<<(std::ostream& os, Token const& token);
}  // namespace flang