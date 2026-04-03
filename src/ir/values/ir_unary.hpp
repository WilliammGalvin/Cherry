#ifndef IR_UNARY_HPP
#define IR_UNARY_HPP
#include <memory>

#include "ir_value.hpp"

namespace cherry::ir {

    enum UnaryOperation {
        NEGATE, LOGICAL_NOT
    };

    struct IRUnary final : IRValue {
        UnaryOperation op;
        std::unique_ptr<IRValue> operand;

        IRUnary(const UnaryOperation op, std::unique_ptr<IRValue> operand)
            : op(op), operand(std::move(operand)) {}
    };

    inline UnaryOperation unary_op_ast_to_ir(const ast::UnaryOp op) {
        switch (op) {
            case ast::UnaryOp::NEGATE: return NEGATE;
                case ast::UnaryOp::LOGICAL_NOT: return LOGICAL_NOT;

            default:
                throw std::runtime_error("Unknown unary operation.");
        }
    }

    inline std::string unary_op_to_str(const UnaryOperation op) {
        switch (op) {
            case NEGATE: return "NEGATE";
            case LOGICAL_NOT: return "LOGICAL_NOT";

            default:
                throw std::runtime_error("Missing string representation of UnaryOperation enum.");
        }
    }

}

#endif //IR_UNARY_HPP
