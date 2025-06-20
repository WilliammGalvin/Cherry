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

        void print(std::ostream& os, int indent) const override {
            os << "UnaryExpr:\n";
            print_indent(os, indent + 1);
            os << "Operator: ";
            switch (op) {
                case UnaryOp::NEGATE:
                    os << "NEGATE\n";
                    break;
                case UnaryOp::LOGICAL_NOT:
                    os << "LOGICAL_NOT\n";
                    break;
            }
            print_indent(os, indent + 1);
            os << "Operand:\n";
            if (operand) {
                operand->print(os, indent + 2);
            } else {
                os << "null\n";
            }
        }
    };

}

#endif //UNARY_OP_HPP
