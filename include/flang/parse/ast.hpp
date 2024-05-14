#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace flang {

enum class ElementNodeType {
  Identifier,
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

class AtomNode;
class IdentifierNode;
class IntegerLiteralNode;
class RealLiteralNode;
class PureListNode;
class QuoteNode;
class SetqNode;
class WhileNode;
class ReturnNode;
class BreakNode;
class FuncNode;
class LambdaNode;
class ProgNode;
class CondNode;
class ProgramNode;

class visitor {
 public:
  virtual void visitIdentifier(IdentifierNode const& node) = 0;
  virtual void visitIntegerLiteral(IntegerLiteralNode const& node) = 0;
  virtual void visitRealLiteral(RealLiteralNode const& node) = 0;
  virtual void visitPureList(PureListNode const& node) = 0;
  virtual void visitQuote(QuoteNode const& node) = 0;
  virtual void visitSetq(SetqNode const& node) = 0;
  virtual void visitWhile(WhileNode const& node) = 0;
  virtual void visitReturn(ReturnNode const& node) = 0;
  virtual void visitBreak(BreakNode const& node) = 0;
  virtual void visitFunc(FuncNode const& node) = 0;
  virtual void visitLambda(LambdaNode const& node) = 0;
  virtual void visitProg(ProgNode const& node) = 0;
  virtual void visitCond(CondNode const& node) = 0;
  virtual void visitProgram(ProgramNode const& node) = 0;
};

// ===== Elements =====

class ElementNode {
  // TODO(furetur): Add location to nodes for error reporting
 public:
  virtual ~ElementNode() {}

  virtual void accept(visitor& visitor) const = 0;

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

  virtual void accept(visitor& visitor) const override { visitor.visitIdentifier(*this); }

  std::string const& name() const { return name_; }

  ElementNodeType type() const override { return ElementNodeType::Identifier; }

 private:
  std::string name_;
};

class IntegerLiteralNode : public AtomNode {
 public:
  IntegerLiteralNode(int value) : value_(value) {}

  virtual ~IntegerLiteralNode() {}

  virtual void accept(visitor& visitor) const override { visitor.visitIntegerLiteral(*this); }

  int value() const { return value_; }

  ElementNodeType type() const override { return ElementNodeType::IntegerLiteral; }

 private:
  int value_;
};

class RealLiteralNode : public AtomNode {
 public:
  RealLiteralNode(double value) : value_(value) {}

  virtual ~RealLiteralNode() {}

  virtual void accept(visitor& visitor) const override { visitor.visitRealLiteral(*this); }

  double value() const { return value_; }

  ElementNodeType type() const override { return ElementNodeType::RealLiteral; }

 private:
  double value_;
};

// --- Lists ---

class PureListNode : public ElementNode {
 public:
  PureListNode(std::vector<std::shared_ptr<ElementNode>> elements) : elements_(std::move(elements)) {}

  virtual ~PureListNode() {}

  virtual void accept(visitor& visitor) const override { visitor.visitPureList(*this); }

  std::vector<std::shared_ptr<ElementNode>> const& elements() const { return elements_; }

  ElementNodeType type() const override { return ElementNodeType::PureList; }

 private:
  std::vector<std::shared_ptr<ElementNode>> elements_;
};

// --- Builtins ---

class QuoteNode : public ElementNode {
 public:
  QuoteNode(std::shared_ptr<ElementNode> arg) : arg_(std::move(arg)) {}

  virtual ~QuoteNode() {}

  virtual void accept(visitor& visitor) const override { visitor.visitQuote(*this); }

  ElementNode const& arg() const { return *arg_; }

  ElementNodeType type() const override { return ElementNodeType::Quote; }

 private:
  std::shared_ptr<ElementNode> arg_;
};

class SetqNode : public ElementNode {
 public:
  SetqNode(std::shared_ptr<IdentifierNode> name, std::shared_ptr<ElementNode> value)
      : name_(std::move(name)), value_(std::move(value)) {}

  virtual ~SetqNode() {}

  virtual void accept(visitor& visitor) const override { visitor.visitSetq(*this); }

  IdentifierNode const& name() const { return *name_; }

  ElementNode const& value() const { return *value_; }

  ElementNodeType type() const override { return ElementNodeType::Setq; }

 private:
  std::shared_ptr<IdentifierNode> name_;
  std::shared_ptr<ElementNode> value_;
};

class WhileNode : public ElementNode {
 public:
  WhileNode(std::shared_ptr<ElementNode> condition, std::shared_ptr<ElementNode> body)
      : condition_(std::move(condition)), body_(std::move(body)) {}

  virtual ~WhileNode(){};

  virtual void accept(visitor& visitor) const override { visitor.visitWhile(*this); }

  ElementNode const& condition() const { return *condition_; }

  ElementNode const& body() const { return *body_; }

  ElementNodeType type() const override { return ElementNodeType::While; }

 private:
  std::shared_ptr<ElementNode> condition_;
  std::shared_ptr<ElementNode> body_;
};

class ReturnNode : public ElementNode {
 public:
  explicit ReturnNode(std::shared_ptr<ElementNode> value) : value_(std::move(value)) {}

  virtual ~ReturnNode(){};

  virtual void accept(visitor& visitor) const override { visitor.visitReturn(*this); }

  ElementNode const& value() const { return *value_; }

  ElementNodeType type() const override { return ElementNodeType::Return; }

 private:
  std::shared_ptr<ElementNode> value_;
};

class BreakNode : public ElementNode {
 public:
  BreakNode() = default;

  virtual ~BreakNode(){};

  virtual void accept(visitor& visitor) const override { visitor.visitBreak(*this); }

  ElementNodeType type() const override { return ElementNodeType::Break; }
};

using FuncFormalArgs = std::vector<std::shared_ptr<IdentifierNode>>;
using FuncBody = std::shared_ptr<ElementNode>;

class FuncNode : public ElementNode {
 public:
  FuncNode(std::shared_ptr<IdentifierNode> name, FuncFormalArgs args, FuncBody body)
      : name_(std::move(name)), args_(std::move(args)), body_(std::move(body)) {}

  virtual ~FuncNode() {}

  virtual void accept(visitor& visitor) const override { visitor.visitFunc(*this); }

  IdentifierNode const& name() const { return *name_; }

  FuncFormalArgs args() const { return args_; }

  FuncBody body() const { return body_; }

  ElementNodeType type() const override { return ElementNodeType::Func; }

 private:
  std::shared_ptr<IdentifierNode> name_;
  FuncFormalArgs args_;
  FuncBody body_;
};

class LambdaNode : public ElementNode {
 public:
  LambdaNode(FuncFormalArgs args, FuncBody body) : args_(std::move(args)), body_(std::move(body)) {}

  virtual ~LambdaNode(){};

  virtual void accept(visitor& visitor) const override { visitor.visitLambda(*this); }

  FuncFormalArgs args() const { return args_; }

  FuncBody body() const { return body_; }

  ElementNodeType type() const override { return ElementNodeType::Lambda; }

 private:
  FuncFormalArgs args_;
  FuncBody body_;
};

class ProgNode : public ElementNode {
 public:
  ProgNode(std::vector<std::shared_ptr<IdentifierNode>> args, std::shared_ptr<ElementNode> body)
      : args_(std::move(args)), body_(std::move(body)) {}

  virtual ~ProgNode(){};

  virtual void accept(visitor& visitor) const override { visitor.visitProg(*this); }

  std::vector<std::shared_ptr<IdentifierNode>> const& args() const { return args_; }

  ElementNode const& body() const { return *body_; }

  ElementNodeType type() const override { return ElementNodeType::Prog; }

 private:
  std::vector<std::shared_ptr<IdentifierNode>> args_;
  std::shared_ptr<ElementNode> body_;
};

class CondNode : public ElementNode {
 public:
  CondNode(std::shared_ptr<ElementNode> condition, std::shared_ptr<ElementNode> then_branch,
           std::shared_ptr<ElementNode> else_branch = nullptr)
      : condition_(std::move(condition)), then_branch_(std::move(then_branch)), else_branch_(std::move(else_branch)) {}

  virtual ~CondNode(){};

  virtual void accept(visitor& visitor) const override { visitor.visitCond(*this); }

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
  std::shared_ptr<ElementNode> condition_;
  std::shared_ptr<ElementNode> then_branch_;
  std::shared_ptr<ElementNode> else_branch_;
};

// ===== Program =====

class ProgramNode {
 public:
  ProgramNode(std::vector<std::shared_ptr<ElementNode>> elements) : elements_(std::move(elements)) {}

  ~ProgramNode() {}

  void accept(visitor& visitor) const { visitor.visitProgram(*this); }

  std::vector<std::shared_ptr<ElementNode>> const& elements() const { return elements_; }

 private:
  std::vector<std::shared_ptr<ElementNode>> elements_;
};

}  // namespace flang
