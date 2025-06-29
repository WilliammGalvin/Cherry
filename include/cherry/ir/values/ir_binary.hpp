#ifndef IR_BINARY_OP_H
#define IR_BINARY_OP_H
#include <stdexcept>

#include "../instructions/ir_instruction.hpp"
#include "ir_value.hpp"

namespace cherry::ir {

    enum class BinaryOperation {
        ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO,
    };

    struct IRBinary final : IRValue {
        BinaryOperation op;
        std::unique_ptr<IRValue> left;
        std::unique_ptr<IRValue> right;

        IRBinary(const BinaryOperation op, std::unique_ptr<IRValue> left, std::unique_ptr<IRValue> right)
            : op(op), left(std::move(left)), right(std::move(right)) {}
    };

    inline BinaryOperation binary_op_ast_to_ir(const ast::BinaryOp op) {
        switch (op) {
            case ast::BinaryOp::ADD: return BinaryOperation::ADD;
                case ast::BinaryOp::SUBTRACT: return BinaryOperation::SUBTRACT;
                case ast::BinaryOp::MULTIPLY: return BinaryOperation::MULTIPLY;
                case ast::BinaryOp::DIVIDE: return BinaryOperation::DIVIDE;
                case ast::BinaryOp::MODULO: return BinaryOperation::MODULO;

            default:
                throw std::runtime_error("Unknown binary operation.");
        }
    }

    inline std::string binary_op_to_str(const BinaryOperation bin_op) {
        switch (bin_op) {
            case BinaryOperation::ADD: return "ADD";
            case BinaryOperation::SUBTRACT: return "SUB";
            case BinaryOperation::MULTIPLY: return "MUL";
            case BinaryOperation::DIVIDE: return "DIV";
            case BinaryOperation::MODULO: return "MOD";

            default:
                throw std::runtime_error("Missing string representation of BinaryOp.");
        }
    }

}

#endif //IR_BINARY_OP_H
