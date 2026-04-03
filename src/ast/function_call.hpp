#ifndef FUNCTION_CALL_EXPR_HPP
#define FUNCTION_CALL_EXPR_HPP
#include <memory>
#include <string>
#include <vector>

#include "expression.hpp"

namespace cherry::ast {

struct FunctionCallExpr final : Expr {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;

  FunctionCallExpr(std::string callee,
                   std::vector<std::unique_ptr<Expr>> &&args)
      : callee(std::move(callee)), args(std::move(args)) {}
};

struct FunctionCallStatement final : Statement {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;

  FunctionCallStatement(std::string callee,
                        std::vector<std::unique_ptr<Expr>> &&args)
      : callee(std::move(callee)), args(std::move(args)) {}
};

} // namespace cherry::ast

#endif // FUNCTION_CALL_EXPR_HPP
