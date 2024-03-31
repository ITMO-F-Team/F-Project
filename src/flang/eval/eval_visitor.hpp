#include <flang/eval/value.hpp>
#include <flang/parse/ast.hpp>
#include <map>

namespace flang {

class EvalVisitor : public visitor {
 public:
  explicit EvalVisitor() : _result(IntegerValue(0)), _variables() { setBuiltinValues(); }

  Value visitElement(ElementNode const& node);

  Value visitElement(const std::unique_ptr<ElementNode>& node);

  virtual void visitIdentifier(IdentifierNode const& node);

  virtual void visitIntegerLiteral(IntegerLiteralNode const& node);

  virtual void visitRealLiteral(RealLiteralNode const& node);

  virtual void visitPureList(PureListNode const& node);

  virtual void visitCall(CallNode const& node);

  virtual void visitQuote(QuoteNode const& node);

  virtual void visitSetq(SetqNode const& node);

  virtual void visitWhile(WhileNode const& node);

  virtual void visitReturn(ReturnNode const& node);

  virtual void visitBreak(BreakNode const& node);

  virtual void visitFunc(FuncNode const& node);

  virtual void visitLambda(LambdaNode const& node);

  virtual void visitProg(ProgNode const& node);

  virtual void visitCond(CondNode const& node);

  virtual void visitProgram(ProgramNode const& node);

 private:
  Value _result;
  std::map<std::string, Value> _variables;

  void setBuiltinValues();

  Value loadVariable(std::string& varname);

  void storeVariable(std::string& varname, Value value);

  void runtimeError(std::string msg);
};

}  // namespace flang
