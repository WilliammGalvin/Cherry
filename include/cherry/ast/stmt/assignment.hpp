#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP
#include <memory>

#include "statement.hpp"
#include "cherry/ast/expr/expression.hpp"
#include "cherry/ast/expr/identifier.hpp"

namespace cherry::ast {

    struct Assignment final : Statement {
        std::unique_ptr<IdentifierExpr> target;
        std::unique_ptr<Expr> value;

        Assignment(std::unique_ptr<IdentifierExpr> target, std::unique_ptr<Expr> value)
            : target(std::move(target)), value(std::move(value)) {}
    };

}

#endif //ASSIGNMENT_HPP
