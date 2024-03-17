#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace flang {

// ===== Elements =====

class ElementNode {
  // TODO(furetur): Add location to nodes for error reporting
 public:
  virtual ~ElementNode() {}
};

// --- Atoms ---

class AtomNode : public ElementNode {
 public:
  virtual ~AtomNode() {}
};

class IdentifierNode : public AtomNode {
 public:
  IdentifierNode(std::string name) : name_(name) {}

  virtual ~IdentifierNode() {}

  std::string& name() const { return (std::string&)name_; }

 private:
  std::string name_;
};

class IntegerLiteralNode : public AtomNode {
 public:
  IntegerLiteralNode(int value) : value_(value) {}

  virtual ~IntegerLiteralNode() {}

  int value() const { return value_; }

 private:
  int value_;
};

class RealLiteralNode : public AtomNode {
 public:
  RealLiteralNode(double value) : value_(value) {}

  virtual ~RealLiteralNode() {}

  int value() const { return value_; }

 private:
  double value_;
};

// --- Lists ---

class PureListNode : public ElementNode {
 public:
  PureListNode(std::vector<std::unique_ptr<ElementNode>> elements) : elements_(std::move(elements)) {}

  ~PureListNode() {}

  std::vector<std::unique_ptr<ElementNode>>& elements() const {
    return (std::vector<std::unique_ptr<ElementNode>>&)elements_;
  }

 private:
  std::vector<std::unique_ptr<ElementNode>> elements_;
};

class CallNode : public ElementNode {
 public:
  CallNode(std::unique_ptr<IdentifierNode> callee, std::vector<std::unique_ptr<ElementNode>> args)
      : callee_(std::move(callee)), args_(std::move(args)) {}

  virtual ~CallNode() {}

  IdentifierNode& callee() const { return *callee_.get(); }

  std::vector<std::unique_ptr<ElementNode>>& args() const { return (std::vector<std::unique_ptr<ElementNode>>&)args_; }

 private:
  std::unique_ptr<IdentifierNode> callee_;
  std::vector<std::unique_ptr<ElementNode>> args_;
};

// --- Builtins ---

class QuoteNode : public ElementNode {
 public:
  QuoteNode(std::unique_ptr<ElementNode> arg) : arg_(std::move(arg)) {}

  virtual ~QuoteNode() {}

  ElementNode& name() const { return (ElementNode&)arg_; }

 private:
  std::unique_ptr<ElementNode> arg_;
};

class SetqNode : public ElementNode {
 public:
  SetqNode(std::unique_ptr<IdentifierNode> name, std::unique_ptr<ElementNode> value)
      : name_(std::move(name)), value_(std::move(value)) {}

  virtual ~SetqNode() {}

  IdentifierNode& name() const { return (IdentifierNode&)name_; }

  ElementNode& value() const { return (ElementNode&)value_; }

 private:
  std::unique_ptr<IdentifierNode> name_;
  std::unique_ptr<ElementNode> value_;
};

class FuncNode : public ElementNode {
 public:
  FuncNode(std::unique_ptr<IdentifierNode> name, std::vector<std::unique_ptr<IdentifierNode>> args,
           std::unique_ptr<ElementNode> body)
      : name_(std::move(name)), args_(std::move(args)), body_(std::move(body)) {}

  virtual ~FuncNode() {}

  IdentifierNode& name() const { return (IdentifierNode&)name_; }

  std::vector<std::unique_ptr<IdentifierNode>>& args() const {
    return (std::vector<std::unique_ptr<IdentifierNode>>&)args_;
  }

  ElementNode& body() const { return (ElementNode&)body_; }

 private:
  std::unique_ptr<IdentifierNode> name_;
  std::vector<std::unique_ptr<IdentifierNode>> args_;
  std::unique_ptr<ElementNode> body_;
};

// ===== Program =====

class ProgramNode {
 public:
  ProgramNode(std::vector<std::unique_ptr<ElementNode>> elements) : elements_(std::move(elements)) {}

  ~ProgramNode() {}

  std::vector<std::unique_ptr<ElementNode>>& elements() const {
    return (std::vector<std::unique_ptr<ElementNode>>&)elements_;
  }

 private:
  std::vector<std::unique_ptr<ElementNode>> elements_;
};

}  // namespace flang
