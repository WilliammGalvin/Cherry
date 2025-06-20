#ifndef RETURN_STATEMENT_HPP
#define RETURN_STATEMENT_HPP

#include <memory>

#include "statement.hpp"
#include "cherry/ast/expr/expression.hpp"

namespace cherry::ast {

    struct ReturnStatement final : Statement {
        std::unique_ptr<Expr> value;

        explicit ReturnStatement(std::unique_ptr<Expr> value) : value(std::move(value)) {}

        void print(std::ostream& os, int indent) const override {
            os << "ReturnStatement:\n";
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

#endif //RETURN_STATEMENT_HPP
