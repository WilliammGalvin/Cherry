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

    static std::string unary_op_to_str(const UnaryOp bin_op) {
        switch (bin_op) {
            case UnaryOp::NEGATE: return "NEGATE";
            case UnaryOp::LOGICAL_NOT: return "NOT";

            default:
                throw std::runtime_error("Missing string representation of BinaryOp.");
        }
    }

}

#endif //UNARY_OP_HPP
