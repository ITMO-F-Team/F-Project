#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace flang
{

class Element;
class Identifier;
class Integer;
class Real;
class Boolean;
class Null;
class List;
class UserFunction;
class Builtin;

using Program = std::vector<std::shared_ptr<Element>>;

class Visitor
{
public:
    virtual void visitProgram(Program program);
    virtual void visitIdentifier(std::shared_ptr<Identifier> node)     = 0;
    virtual void visitInteger(std::shared_ptr<Integer> node)           = 0;
    virtual void visitReal(std::shared_ptr<Real> node)                 = 0;
    virtual void visitBoolean(std::shared_ptr<Boolean> node)           = 0;
    virtual void visitNull(std::shared_ptr<Null> node)                 = 0;
    virtual void visitList(std::shared_ptr<List> node)                 = 0;
    virtual void visitUserFunction(std::shared_ptr<UserFunction> node) = 0;
    virtual void visitBuiltin(std::shared_ptr<Builtin> node)           = 0;
};

class EvalVisitor;

class Element
{
public:
    virtual ~Element() = default;

    virtual void accept(Visitor& visitor) = 0;

private:
    // TODO: Location
};

class Identifier : public Element, public std::enable_shared_from_this<Identifier>
{
public:
    explicit Identifier(std::string name)
        : name_(std::move(name))
    {
    }

    std::string getName() const
    {
        return name_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitIdentifier(shared_from_this());
    }


private:
    std::string name_;
};

class Literal : public Element
{
public:
    virtual ~Literal() = default;
};

class Integer final : public Literal, public std::enable_shared_from_this<Integer>
{
public:
    using internal_type_t = int64_t;

    explicit Integer(int64_t value)
        : value_(value)
    {
    }

    internal_type_t getValue() const
    {
        return value_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitInteger(shared_from_this());
    }

private:
    internal_type_t value_;
};

class Real final : public Literal, public std::enable_shared_from_this<Real>
{
public:
    using internal_type_t = double;

    explicit Real(double value)
        : value_(value)
    {
    }

    internal_type_t getValue() const
    {
        return value_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitReal(shared_from_this());
    }

private:
    internal_type_t value_;
};

class Boolean final : public Literal, public std::enable_shared_from_this<Boolean>
{
public:
    using internal_type_t = bool;

    explicit Boolean(bool value)
        : value_(value)
    {
    }

    internal_type_t getValue() const
    {
        return value_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitBoolean(shared_from_this());
    }

private:
    internal_type_t value_;
};

class Null final : public Literal, public std::enable_shared_from_this<Null>
{
    void accept(Visitor& visitor) override
    {
        visitor.visitNull(shared_from_this());
    }
};

class List final : public Element, public std::enable_shared_from_this<List>
{
public:
    explicit List(std::vector<std::shared_ptr<Element>> elements)
        : elements_(std::move(elements))
    {
    }

    auto const& getElements() const
    {
        return elements_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitList(shared_from_this());
    }

private:
    std::vector<std::shared_ptr<Element>> elements_;
};

class UserFunction final : public Element, public std::enable_shared_from_this<UserFunction>
{
public:
    UserFunction(std::string name, std::vector<std::string> formal_args, std::shared_ptr<Element> body)
        : name_(name)
        , formal_args_(formal_args)
        , body_(body)
    {
    }

    std::string getName() const
    {
        return name_;
    }

    std::vector<std::string> const& getFormalArgs() const
    {
        return formal_args_;
    }

    std::shared_ptr<Element> getBody() const
    {
        return body_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitUserFunction(shared_from_this());
    }

private:
    std::string name_;
    std::vector<std::string> formal_args_;
    std::shared_ptr<Element> body_;
};

class Builtin final : public Element, public std::enable_shared_from_this<Builtin>
{
public:
    explicit Builtin(std::string name)
        : name_(name)
    {
    }

    std::string getName() const
    {
        return name_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitBuiltin(shared_from_this());
    }

private:
    std::string name_;
};


} // namespace flang
