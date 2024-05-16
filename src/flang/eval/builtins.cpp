#include "flang/eval/builtins.hpp"
#include <algorithm>
#include <flang/eval/environment_stack.hpp>
#include <flang/flang_exception.hpp>
#include <flang/parse/ast.hpp>
#include <flang/pp/ast_printer.hpp>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>


namespace flang
{

void print_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    std::cout << printElement(visitor->evalElement(args[0]));
}

void assert_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    auto val = visitor->requireBoolean(visitor->evalElement(args[0]));
    if (!val->getValue()) {
        visitor->throwRuntimeError("Assertion error!");
    }
}

void setq_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 2);
    auto id  = visitor->requireIdentifier(args[0]);
    auto val = visitor->evalElement(args[1]);
    visitor->storeVariable(id->getName(), val);
}

void cond_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    if ((args.size() != 2) && (args.size() != 3)) {
        visitor->throwRuntimeError("cond expects 2-3 arguments");
    }
    auto cond = visitor->requireBoolean(visitor->evalElement(args[0]));
    if (cond->getValue()) {
        visitor->evalElement(args[1]);
    } else {
        if (args.size() == 3) {
            visitor->evalElement(args[2]);
        }
    }
}


template <bool isMacro>
void func_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 3);
    auto id        = visitor->requireIdentifier(args[0]);
    auto args_list = visitor->requireList(args[1])->getElements();
    auto body      = args[2];

    std::vector<std::string> formal_args;
    std::transform(args_list.begin(), args_list.end(), std::back_inserter(formal_args), [visitor](auto&& x) {
        auto id = visitor->requireIdentifier(x);
        return id->getName();
    });

    auto fn = std::make_shared<UserFunction>(id->getName(), formal_args, body, isMacro);
    visitor->storeVariable(id->getName(), fn);
}

void lambda_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 2);
    auto args_list = visitor->requireList(args[0])->getElements();
    auto body      = args[1];

    std::vector<std::string> formal_args;
    std::transform(args_list.begin(), args_list.end(), std::back_inserter(formal_args), [visitor](auto&& x) {
        auto id = visitor->requireIdentifier(x);
        return id->getName();
    });

    auto fn = std::make_shared<UserFunction>("anonymous lambda", formal_args, body);
    visitor->setResult(fn);
}

void return_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    visitor->setResult(visitor->evalElement(args[0]));
    throw flang_return();
}

void break_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 0);
    throw flang_break();
}

void while_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 2);
    auto cond = args[0];
    while (visitor->requireBoolean(visitor->evalElement(cond))->getValue()) {
        try {
            visitor->evalElement(args[1]);
        } catch (flang_break const& e) {
            break;
        }
    }
    visitor->setNullResult();
}

void quote_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    visitor->setResult(args[0]);
}

void plus_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 2);
    auto lhs = visitor->requireInteger(visitor->evalElement(args[0]));
    auto rhs = visitor->requireInteger(visitor->evalElement(args[1]));
    visitor->setResult(std::make_shared<Integer>(lhs->getValue() + rhs->getValue()));
}

void head_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    auto list = visitor->requireList(visitor->evalElement(args[0]));
    if (list->getElements().empty()) {
        visitor->setResult(std::make_shared<Null>());
    } else {
        visitor->setResult(list->getElements()[0]);
    }
}

void tail_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    auto list = visitor->requireList(visitor->evalElement(args[0]));
    if (list->getElements().size() <= 1) {
        visitor->setResult(std::make_shared<Null>());
    } else {
        visitor->setResult(std::make_shared<List>(std::vector<std::shared_ptr<Element>>(list->getElements().begin() + 1, list->getElements().end())));
    }
}

void cons_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 2);

    auto head = visitor->evalElement(args[0]);
    auto list = visitor->requireList(visitor->evalElement(args[1]));

    std::vector<std::shared_ptr<Element>> concat_list = {head};
    std::copy(list->getElements().begin(), list->getElements().end(), std::back_inserter(concat_list));

    visitor->setResult(std::make_shared<List>(std::move(concat_list)));
}

template <class T>
void is_type_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);

    auto evaluated_arg = visitor->evalElement(args[0]);
    visitor->setResult(std::make_shared<Boolean>(std::dynamic_pointer_cast<T>(evaluated_arg) != nullptr));
}

template <class ReturnType, class RequiredType, template <typename T = RequiredType::internal_type_t> class BinOp>
void binop_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 2);

    auto lhs = visitor->evalElement(args[0]);
    auto rhs = visitor->evalElement(args[1]);

    // TODO: Make require templated
    if constexpr (std::is_same_v<RequiredType, Integer>) {
        lhs = visitor->requireInteger(lhs);
        rhs = visitor->requireInteger(rhs);
    } else {
        lhs = visitor->requireBoolean(lhs);
        rhs = visitor->requireBoolean(rhs);
    }

    auto result_value = BinOp()(std::dynamic_pointer_cast<RequiredType>(lhs)->getValue(), std::dynamic_pointer_cast<RequiredType>(rhs)->getValue());
    visitor->setResult(std::make_shared<ReturnType>(result_value));
}

void prog_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    // 1. Check and collect args
    visitor->requireArgsNumber(args, 2);
    auto context_list = visitor->requireList(args[0])->getElements();
    auto body         = visitor->requireList(args[1])->getElements();
    std::vector<std::string> context_ids;
    std::transform(context_list.begin(), context_list.end(), std::back_inserter(context_ids), [visitor](auto&& x) {
        auto id = visitor->requireIdentifier(x);
        return id->getName();
    });
    // 2. Create environment
    auto env = visitor->createScopedEnvironment();
    // 3. Add context variables
    for (auto&& id : context_ids) {
        visitor->storeVariable(id, std::make_shared<Null>());
    }
    // 4. Eval body
    for (auto item : body) {
        visitor->evalElement(item);
    }
}

void eval_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    // 1. Eval passed argument (as a regular function call)
    auto first_result = visitor->evalElement(args[0]);
    // 2. Eval as requested
    visitor->evalElement(first_result);
}

template <class EqualityOp>
void equal_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 2);

    auto lhs = visitor->evalElement(args[0]);
    auto rhs = visitor->evalElement(args[1]);

    bool result_value = false;

    if (auto lhsBoolean = std::dynamic_pointer_cast<Boolean>(lhs)) {
        if (auto rhsBoolean = std::dynamic_pointer_cast<Boolean>(rhs)) {
            result_value = EqualityOp()(lhsBoolean->getValue(), rhsBoolean->getValue());
        }
    } else if (auto lhsInteger = std::dynamic_pointer_cast<Integer>(lhs)) {
        if (auto rhsInteger = std::dynamic_pointer_cast<Integer>(rhs)) {
            result_value = EqualityOp()(lhsInteger->getValue(), rhsInteger->getValue());
        }
    }

    visitor->setResult(std::make_shared<Boolean>(result_value));
}

void not_impl(EvalVisitor* visitor, std::vector<std::shared_ptr<Element>> args)
{
    visitor->requireArgsNumber(args, 1);
    auto argument = visitor->requireBoolean(visitor->evalElement(args[0]));
    visitor->setResult(std::make_shared<Boolean>(!argument->getValue()));
}

// ====== Builtins Registry =====

std::vector<std::shared_ptr<Builtin>> BuiltinsRegistry::getAllBuiltins()
{
    std::vector<std::shared_ptr<Builtin>> result;
    for (auto const& entry : registry_) {
        result.emplace_back(std::make_shared<Builtin>(entry.first));
    }
    return result;
}

void BuiltinsRegistry::callBuiltin(std::shared_ptr<Builtin> builtin, std::vector<std::shared_ptr<Element>> args)
{
    if (!registry_.contains(builtin->getName())) {
        throw std::runtime_error("Builtin not found: " + builtin->getName());
    }
    auto impl = registry_.at(builtin->getName());
    impl(visitor_, args);
}

void BuiltinsRegistry::registerAllBuiltins()
{
    registry_.insert_or_assign("print", print_impl);
    registry_.insert_or_assign("assert", assert_impl);
    registry_.insert_or_assign("setq", setq_impl);
    registry_.insert_or_assign("cond", cond_impl);
    registry_.insert_or_assign("func", func_impl<false>);
    registry_.insert_or_assign("macro", func_impl<true>);

    registry_.insert_or_assign("lambda", lambda_impl);
    registry_.insert_or_assign("prog", prog_impl);
    registry_.insert_or_assign("eval", eval_impl);

    registry_.insert_or_assign("return", return_impl);
    registry_.insert_or_assign("break", break_impl);
    registry_.insert_or_assign("while", while_impl);
    registry_.insert_or_assign("quote", quote_impl);

    registry_.insert_or_assign("head", head_impl);
    registry_.insert_or_assign("tail", tail_impl);
    registry_.insert_or_assign("cons", cons_impl);

    registry_.insert_or_assign("isint", is_type_impl<Integer>);
    registry_.insert_or_assign("isreal", is_type_impl<Real>);
    registry_.insert_or_assign("isbool", is_type_impl<Boolean>);
    registry_.insert_or_assign("isnull", is_type_impl<Null>);
    registry_.insert_or_assign("isatom", is_type_impl<Identifier>);
    registry_.insert_or_assign("islist", is_type_impl<List>);

    registry_.insert_or_assign("plus", binop_impl<Integer, Integer, std::plus>);
    registry_.insert_or_assign("minus", binop_impl<Integer, Integer, std::minus>);
    registry_.insert_or_assign("times", binop_impl<Integer, Integer, std::multiplies>);
    // TODO: Null division exception
    registry_.insert_or_assign("divide", binop_impl<Integer, Integer, std::divides>);

    registry_.insert_or_assign("less", binop_impl<Boolean, Integer, std::less>);
    registry_.insert_or_assign("lesseq", binop_impl<Boolean, Integer, std::less_equal>);
    registry_.insert_or_assign("greater", binop_impl<Boolean, Integer, std::greater>);
    registry_.insert_or_assign("greatereq", binop_impl<Boolean, Integer, std::greater_equal>);

    registry_.insert_or_assign("and", binop_impl<Boolean, Boolean, std::logical_and>);
    registry_.insert_or_assign("or", binop_impl<Boolean, Boolean, std::logical_or>);
    registry_.insert_or_assign("xor", binop_impl<Boolean, Boolean, std::bit_xor>);

    registry_.insert_or_assign("equal", equal_impl<std::equal_to<>>);
    registry_.insert_or_assign("nonequal", equal_impl<std::not_equal_to<>>);
    registry_.insert_or_assign("not", not_impl);
}

} // namespace flang
