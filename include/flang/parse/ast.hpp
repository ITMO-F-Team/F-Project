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
class Function;

class Visitor
{
public:
    virtual void visitIdentifier(std::shared_ptr<Identifier> node) = 0;
    virtual void visitInteger(std::shared_ptr<Integer> node)       = 0;
    virtual void visitReal(std::shared_ptr<Real> node)             = 0;
    virtual void visitBoolean(std::shared_ptr<Boolean> node)       = 0;
    virtual void visitNull(std::shared_ptr<Null> node)             = 0;
    virtual void visitList(std::shared_ptr<List> node)             = 0;
    virtual void visitFunction(std::shared_ptr<Function> node)     = 0;
};

class Element
{
public:
    virtual ~Element() = default;

    virtual void accept(Visitor& visitor) = 0;

private:
    // TODO: Location
};

using Program = std::vector<std::shared_ptr<Element>>;

class Identifier : public Element, std::enable_shared_from_this<Identifier>
{
public:
    static Identifier LambdaIdentifier()
    {
        return Identifier("LAMBDA");
    }

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

class Integer final : public Literal, std::enable_shared_from_this<Integer>
{
public:
    explicit Integer(int64_t value)
        : value_(value)
    {
    }

    int64_t getValue() const
    {
        return value_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitInteger(shared_from_this());
    }

private:
    int64_t value_;
};

class Real final : public Literal, std::enable_shared_from_this<Real>
{
public:
    explicit Real(double value)
        : value_(value)
    {
    }

    double getValue() const
    {
        return value_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitReal(shared_from_this());
    }

private:
    double value_;
};

class Boolean final : public Literal, std::enable_shared_from_this<Boolean>
{
public:
    explicit Boolean(bool value)
        : value_(value)
    {
    }

    bool getValue() const
    {
        return value_;
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitBoolean(shared_from_this());
    }

private:
    bool value_;
};

class Null final : public Literal, std::enable_shared_from_this<Null>
{
    void accept(Visitor& visitor) override
    {
        visitor.visitNull(shared_from_this());
    }
};

class List final : public Element, std::enable_shared_from_this<List>
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

class Function final : public Element, std::enable_shared_from_this<Function>
{
public:
    Function(std::shared_ptr<Identifier> identifier, std::function<void(Visitor&, std::vector<std::shared_ptr<Element>>)> impl)
        : identifier_(std::move(identifier))
        , impl_(impl)
    {
    }

    template <class... Args>
    void call(Args&&... args)
    {
        impl_(std::forward<Args>(args)...);
    }

    void accept(Visitor& visitor) override
    {
        visitor.visitFunction(shared_from_this());
    }

private:
    std::shared_ptr<Identifier> identifier_;
    std::function<void(Visitor&, std::vector<std::shared_ptr<Element>>)> impl_;
};

} // namespace flang
