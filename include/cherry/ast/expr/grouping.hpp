#ifndef GROUPING_HPP
#define GROUPING_HPP
#include <memory>

#include "expression.hpp"

namespace cherry::ast {

    struct GroupingExpr final : Expr {
        std::unique_ptr<Expr> inner;

        explicit GroupingExpr(std::unique_ptr<Expr> inner) : inner(std::move(inner)) {}

        void print(std::ostream& os, int indent) const override {
            os << "GroupingExpr:\n";
            print_indent(os, indent + 1);
            os << "Inner Expression:\n";
            if (inner) {
                inner->print(os, indent + 2);
            } else {
                os << "null\n";
            }
        }
    };

}

#endif //GROUPING_HPP
