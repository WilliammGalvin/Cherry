#ifndef FOR_STATEMENT_HPP
#define FOR_STATEMENT_HPP

#include <memory>
#include <vector>

#include "statement.hpp"
#include "cherry/ast/expr/expression.hpp"

namespace cherry::ast {

    struct ForStatement final : Statement {
        std::unique_ptr<Statement> initializer;
        std::unique_ptr<Expr> condition;
        std::unique_ptr<Assignment> increment;
        std::vector<std::unique_ptr<Statement>> body;

        ForStatement(
            std::unique_ptr<Statement> initializer,
            std::unique_ptr<Expr> condition,
            std::unique_ptr<Assignment> increment,
            std::vector<std::unique_ptr<Statement>>&& body
        ) :
            initializer(std::move(initializer)),
            condition(std::move(condition)),
            increment(std::move(increment)),
            body(std::move(body)) {}
    };

}

#endif //FOR_STATEMENT_HPP
