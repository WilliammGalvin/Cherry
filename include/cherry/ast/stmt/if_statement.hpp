#ifndef IF_STATEMENT_HPP
#define IF_STATEMENT_HPP
#include <memory>
#include <vector>

#include "statement.hpp"
#include "cherry/ast/expr/expression.hpp"

namespace cherry::ast {

    struct IfStatement final : Statement {
        std::unique_ptr<Expr> condition;
        std::vector<std::unique_ptr<Statement>> then_statements;
        std::vector<std::unique_ptr<Statement>> else_statements;

        IfStatement(std::unique_ptr<Expr> condition, std::vector<std::unique_ptr<Statement>> then_statements,
                    std::vector<std::unique_ptr<Statement>> else_statements)
            : condition(std::move(condition)), then_statements(std::move(then_statements)),
              else_statements(std::move(else_statements)) {}

        void print(std::ostream& os, int indent) const override {
            os << "IfStatement:\n";
            print_indent(os, indent + 1);
            os << "Condition:\n";
            if (condition) {
                condition->print(os, indent + 2);
            } else {
                os << "null\n";
            }
            print_indent(os, indent + 1);
            os << "Then Statements:\n";
            for (const auto& stmt : then_statements) {
                if (stmt) {
                    stmt->print(os, indent + 2);
                } else {
                    os << "null\n";
                }
            }
            print_indent(os, indent + 1);
            os << "Else Statements:\n";
            for (const auto& stmt : else_statements) {
                if (stmt) {
                    stmt->print(os, indent + 2);
                } else {
                    os << "null\n";
                }
            }
        }
    };

}

#endif //IF_STATEMENT_HPP
