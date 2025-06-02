#ifndef GROUPING_HPP
#define GROUPING_HPP
#include <memory>

#include "expression.hpp"

namespace cherry::ast {

    struct GroupingExpr final : Expr {
        std::unique_ptr<Expr> inner;

        explicit GroupingExpr(std::unique_ptr<Expr> inner) : inner(std::move(inner)) {}
    };

}

#endif //GROUPING_HPP
