#ifndef WHILE_LOOP_HPP
#define WHILE_LOOP_HPP
#include <memory>
#include <vector>

#include "statement.hpp"
#include "cherry/ast/expr/expression.hpp"

namespace cherry::ast {

    struct WhileStatement final : Statement {
        std::unique_ptr<Expr> condition;
        std::vector<std::unique_ptr<Statement>> body;

        WhileStatement(std::unique_ptr<Expr> cond, std::vector<std::unique_ptr<Statement>> body)
            : condition(std::move(cond)), body(std::move(body)) {}
    };

}

#endif //WHILE_LOOP_HPP
