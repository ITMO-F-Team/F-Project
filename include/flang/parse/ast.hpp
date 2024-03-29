#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace flang {

enum class ElementNodeType {
  Identifier,
  Call,
  IntegerLiteral,
  RealLiteral,
  Setq,
  Cond,
  While,
  PureList,
  Prog,
  Func,
  Return,
  Lambda,
  Quote,
  Break
};

// ===== Elements =====

class ElementNode {
  // TODO(furetur): Add location to nodes for error reporting
 public:
  virtual ~ElementNode() {}

  virtual ElementNodeType type() const = 0;
};

// --- Atoms ---

class AtomNode : public ElementNode {
 public:
  virtual ~AtomNode() {}
};

class IdentifierNode : public AtomNode {
 public:
  IdentifierNode(std::string name) : name_(std::move(name)) {}

  virtual ~IdentifierNode() {}

  std::string const& name() const { return name_; }

  ElementNodeType type() const override { return ElementNodeType::Identifier; }

 private:
  std::string name_;
};

class IntegerLiteralNode : public AtomNode {
 public:
  IntegerLiteralNode(int value) : value_(value) {}

  virtual ~IntegerLiteralNode() {}

  int value() const { return value_; }

  ElementNodeType type() const override { return ElementNodeType::IntegerLiteral; }

 private:
  int value_;
};

class RealLiteralNode : public AtomNode {
 public:
  RealLiteralNode(double value) : value_(value) {}

  virtual ~RealLiteralNode() {}

  double value() const { return value_; }

  ElementNodeType type() const override { return ElementNodeType::RealLiteral; }

 private:
  double value_;
};

// --- Lists ---

class PureListNode : public ElementNode {
 public:
  PureListNode(std::vector<std::unique_ptr<ElementNode>> elements) : elements_(std::move(elements)) {}

  ~PureListNode() {}

  std::vector<std::unique_ptr<ElementNode>> const& elements() const { return elements_; }

  ElementNodeType type() const override { return ElementNodeType::PureList; }

 private:
  std::vector<std::unique_ptr<ElementNode>> elements_;
};

class CallNode : public ElementNode {
 public:
  CallNode(std::unique_ptr<IdentifierNode> callee, std::vector<std::unique_ptr<ElementNode>> args)
      : callee_(std::move(callee)), args_(std::move(args)) {}

  virtual ~CallNode() {}

  IdentifierNode const& callee() const { return *callee_; }

  std::vector<std::unique_ptr<ElementNode>> const& args() const { return args_; }

  ElementNodeType type() const override { return ElementNodeType::Call; }

 private:
  std::unique_ptr<IdentifierNode> callee_;
  std::vector<std::unique_ptr<ElementNode>> args_;
};

// --- Builtins ---

class QuoteNode : public ElementNode {
 public:
  QuoteNode(std::unique_ptr<ElementNode> arg) : arg_(std::move(arg)) {}

  virtual ~QuoteNode() {}

  ElementNode const& arg() const { return *arg_; }

  ElementNodeType type() const override { return ElementNodeType::Quote; }

 private:
  std::unique_ptr<ElementNode> arg_;
};

class SetqNode : public ElementNode {
 public:
  SetqNode(std::unique_ptr<IdentifierNode> name, std::unique_ptr<ElementNode> value)
      : name_(std::move(name)), value_(std::move(value)) {}

  virtual ~SetqNode() {}

  IdentifierNode const& name() const { return *name_; }

  ElementNode const& value() const { return *value_; }

  ElementNodeType type() const override { return ElementNodeType::Setq; }

 private:
  std::unique_ptr<IdentifierNode> name_;
  std::unique_ptr<ElementNode> value_;
};

class WhileNode : public ElementNode {
 public:
  WhileNode(std::unique_ptr<ElementNode> condition, std::unique_ptr<ElementNode> body)
      : condition_(std::move(condition)), body_(std::move(body)) {}

  virtual ~WhileNode(){};

  ElementNode const& condition() const { return *condition_; }

  ElementNode const& body() const { return *body_; }

  ElementNodeType type() const override { return ElementNodeType::While; }

 private:
  std::unique_ptr<ElementNode> condition_;
  std::unique_ptr<ElementNode> body_;
};

class ReturnNode : public ElementNode {
 public:
  explicit ReturnNode(std::unique_ptr<ElementNode> value) : value_(std::move(value)) {}

  virtual ~ReturnNode(){};

  ElementNode const& value() const { return *value_; }

  ElementNodeType type() const override { return ElementNodeType::Return; }

 private:
  std::unique_ptr<ElementNode> value_;
};

class BreakNode : public ElementNode {
 public:
  BreakNode() = default;

  virtual ~BreakNode(){};

  ElementNodeType type() const override { return ElementNodeType::Break; }
};

class FuncNode : public ElementNode {
 public:
  FuncNode(std::unique_ptr<IdentifierNode> name, std::vector<std::unique_ptr<IdentifierNode>> args,
           std::unique_ptr<ElementNode> body)
      : name_(std::move(name)), args_(std::move(args)), body_(std::move(body)) {}

  virtual ~FuncNode() {}

  IdentifierNode const& name() const { return *name_; }

  std::vector<std::unique_ptr<IdentifierNode>> const& args() const { return args_; }

  ElementNode const& body() const { return *body_; }

  ElementNodeType type() const override { return ElementNodeType::Func; }

 private:
  std::unique_ptr<IdentifierNode> name_;
  std::vector<std::unique_ptr<IdentifierNode>> args_;
  std::unique_ptr<ElementNode> body_;
};

class LambdaNode : public ElementNode {
 public:
  LambdaNode(std::vector<std::unique_ptr<IdentifierNode>> args, std::unique_ptr<ElementNode> body)
      : args_(std::move(args)), body_(std::move(body)) {}

  virtual ~LambdaNode(){};

  std::vector<std::unique_ptr<IdentifierNode>> const& args() const { return args_; }

  ElementNode const& body() const { return *body_; }

  ElementNodeType type() const override { return ElementNodeType::Lambda; }

 private:
  std::vector<std::unique_ptr<IdentifierNode>> args_;
  std::unique_ptr<ElementNode> body_;
};

class ProgNode : public ElementNode {
 public:
  ProgNode(std::vector<std::unique_ptr<IdentifierNode>> args, std::unique_ptr<ElementNode> body)
      : args_(std::move(args)), body_(std::move(body)) {}

  virtual ~ProgNode(){};

  std::vector<std::unique_ptr<IdentifierNode>> const& args() const { return args_; }

  ElementNode const& body() const { return *body_; }

  ElementNodeType type() const override { return ElementNodeType::Prog; }

 private:
  std::vector<std::unique_ptr<IdentifierNode>> args_;
  std::unique_ptr<ElementNode> body_;
};

class CondNode : public ElementNode {
 public:
  CondNode(std::unique_ptr<ElementNode> condition, std::unique_ptr<ElementNode> then_branch,
           std::unique_ptr<ElementNode> else_branch = nullptr)
      : condition_(std::move(condition)), then_branch_(std::move(then_branch)), else_branch_(std::move(else_branch)) {}

  virtual ~CondNode(){};

  ElementNode const& condition() const { return *condition_; }

  ElementNode const& thenBranch() const { return *then_branch_; }

  std::optional<std::reference_wrapper<const ElementNode>> elseBranch() const {
    if (else_branch_) {
      return std::cref(*else_branch_);
    } else {
      return std::nullopt;
    }
  };

  ElementNodeType type() const override { return ElementNodeType::Cond; }

 private:
  std::unique_ptr<ElementNode> condition_;
  std::unique_ptr<ElementNode> then_branch_;
  std::unique_ptr<ElementNode> else_branch_;
};

// ===== Program =====

class ProgramNode {
 public:
  ProgramNode(std::vector<std::unique_ptr<ElementNode>> elements) : elements_(std::move(elements)) {}

  ~ProgramNode() {}

  std::vector<std::unique_ptr<ElementNode>> const& elements() const { return elements_; }

 private:
  std::vector<std::unique_ptr<ElementNode>> elements_;
};

}  // namespace flang
