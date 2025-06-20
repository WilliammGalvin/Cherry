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

        void print(std::ostream& os, int indent) const override {
            os << "Assignment:\n";
            print_indent(os, indent + 1);
            os << "Target:\n";
            if (target) {
                target->print(os, indent + 2);
            } else {
                os << "null\n";
            }
            print_indent(os, indent + 1);
            os << "Value:\n";
            if (value) {
                value->print(os, indent + 2);
            } else {
                os << "null\n";
            }
        }
    };

}

#endif //ASSIGNMENT_HPP
