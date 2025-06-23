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

    static std::string binary_op_to_str(const BinaryOp bin_op) {
        switch (bin_op) {
            case BinaryOp::ADD: return "ADD";
            case BinaryOp::SUBTRACT: return "SUBTRACT";
            case BinaryOp::MULTIPLY: return "MULTIPLY";
            case BinaryOp::DIVIDE: return "DIVIDE";
            case BinaryOp::MODULO: return "MODULO";
            case BinaryOp::EQUAL: return "EQUAL";
            case BinaryOp::NOT_EQUAL: return "NOT_EQUAL";
            case BinaryOp::LESS_THAN: return "LESS_THAN";
            case BinaryOp::LESS_EQUAL: return "LESS_EQUAL";
            case BinaryOp::GREATER_THAN: return "GREATER_THAN";
            case BinaryOp::GREATER_EQUAL: return "GREATER_EQUAL";
            case BinaryOp::LOGICAL_AND: return "LOGICAL_AND";
            case BinaryOp::LOGICAL_OR: return "LOGICAL_OR";

            default:
                throw std::runtime_error("Missing string representation of BinaryOp.");
        }
    }

}

#endif //BINARY_OP_HPP
