#pragma once

#include <memory>

#include "flang/parse/ast.hpp"

namespace flang
{

class Element;
class Identifier;
class Integer;
class Real;
class Boolean;
class Null;
class List;

class Visitor
{
public:
    virtual void visitIdentifier(std::shared_ptr<Identifier> node) = 0;
    virtual void visitInteger(std::shared_ptr<Integer> node)       = 0;
    virtual void visitReal(std::shared_ptr<Real> node)             = 0;
    virtual void visitBoolean(std::shared_ptr<Boolean> node)       = 0;
    virtual void visitNull(std::shared_ptr<Null> node)             = 0;
    virtual void visitList(std::shared_ptr<List> node)             = 0;
};

} // namespace flang
