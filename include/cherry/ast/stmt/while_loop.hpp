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

        void print(std::ostream& os, int indent) const override {
            os << "WhileStatement:\n";
            print_indent(os, indent + 1);
            os << "Condition:\n";
            if (condition) {
                condition->print(os, indent + 2);
            } else {
                os << "null\n";
            }
            print_indent(os, indent + 1);
            os << "Body:\n";
            for (const auto& stmt : body) {
                if (stmt) {
                    stmt->print(os, indent + 2);
                } else {
                    os << "null\n";
                }
            }
        }
    };

}

#endif //WHILE_LOOP_HPP
