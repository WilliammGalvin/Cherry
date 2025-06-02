#ifndef BINARY_OP_HPP
#define BINARY_OP_HPP
#include <memory>

#include "expression.hpp"

namespace cherry::ast {

    enum class BinaryOp {
        ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO,
        EQUAL, NOT_EQUAL, LESS_THAN, LESS_EQUAL, GREATER_THAN, GREATER_EQUAL,
        LOGICAL_AND, LOGICAL_OR
    };

    struct BinaryExpr final : Expr {
        BinaryOp op;
        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;

        BinaryExpr(const BinaryOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
            : op(op), left(std::move(left)), right(std::move(right)) {}
    };

}

#endif //BINARY_OP_HPP
