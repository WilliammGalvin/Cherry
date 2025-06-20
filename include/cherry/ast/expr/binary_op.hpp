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

        void print(std::ostream& os, int indent) const override {
            os << std::string(indent, ' ') << "BinaryExpr: " << static_cast<int>(op) << "\n";
            if (left) {
                os << std::string(indent + 2, ' ') << "Left:\n";
                left->print(os, indent + 4);
            }
            if (right) {
                os << std::string(indent + 2, ' ') << "Right:\n";
                right->print(os, indent + 4);
            }
        }
    };

}

#endif //BINARY_OP_HPP
