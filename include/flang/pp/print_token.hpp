#pragma once

#include <iostream>
#include <vector>

#include "flang/tokenize/token.hpp"

namespace flang
{
std::ostream& operator<<(std::ostream& os, Token const& token);
} // namespace flang
