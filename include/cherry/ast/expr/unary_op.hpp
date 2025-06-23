#ifndef UNARY_OP_HPP
#define UNARY_OP_HPP
#include <memory>

#include "expression.hpp"

namespace cherry::ast {

    enum class UnaryOp {
        NEGATE,
        LOGICAL_NOT,
    };

    struct UnaryExpr final : Expr {
        UnaryOp op;
        std::unique_ptr<Expr> operand;

        UnaryExpr(const UnaryOp op, std::unique_ptr<Expr> operand)
            : op(op), operand(std::move(operand)) {}
    };

}

#endif //UNARY_OP_HPP
