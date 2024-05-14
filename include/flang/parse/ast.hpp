#pragma once

#include <memory>
#include <string>
#include <vector>

namespace flang
{
class Element
{
public:
    virtual ~Element() = default;

private:
    // TODO: Location
};

using Program = std::vector<std::shared_ptr<Element>>;

class Identifier : public Element
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

private:
    std::string name_;
};

class Literal : public Element
{
public:
    virtual ~Literal() = default;
};

class Integer final : public Literal
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

private:
    int64_t value_;
};

class Real final : public Literal
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

private:
    double value_;
};

class Boolean final : public Literal
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

private:
    bool value_;
};

class Null final : public Literal
{
};

class List final : public Element
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

private:
    std::vector<std::shared_ptr<Element>> elements_;
};

} // namespace flang