#ifndef RETURN_STATEMENT_HPP
#define RETURN_STATEMENT_HPP

#include <memory>

#include "statement.hpp"
#include "cherry/ast/expr/expression.hpp"

namespace cherry::ast {

    struct ReturnStatement final : Statement {
        std::unique_ptr<Expr> value;

        explicit ReturnStatement(std::unique_ptr<Expr> value) : value(std::move(value)) {}
    };

}

#endif //RETURN_STATEMENT_HPP
