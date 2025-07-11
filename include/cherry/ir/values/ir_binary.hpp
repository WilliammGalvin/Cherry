#ifndef IR_BINARY_OP_H
#define IR_BINARY_OP_H
#include <stdexcept>

#include "../instructions/ir_instruction.hpp"
#include "ir_value.hpp"

namespace cherry::ir {

    enum class BinaryOperation {
        ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO,
        LOGICAL_OR, LOGICAL_AND,
        EQUAL, NOT_EQUAL,
        GREATER, LESS, GREATER_EQUAL, LESS_EQUAL,
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

            case ast::BinaryOp::LOGICAL_OR: return BinaryOperation::LOGICAL_OR;
            case ast::BinaryOp::LOGICAL_AND: return BinaryOperation::LOGICAL_AND;
            case ast::BinaryOp::EQUAL: return BinaryOperation::EQUAL;
            case ast::BinaryOp::NOT_EQUAL: return BinaryOperation::NOT_EQUAL;
            case ast::BinaryOp::GREATER_THAN: return BinaryOperation::GREATER;
            case ast::BinaryOp::LESS_THAN: return BinaryOperation::LESS;
            case ast::BinaryOp::GREATER_EQUAL: return BinaryOperation::GREATER_EQUAL;
            case ast::BinaryOp::LESS_EQUAL: return BinaryOperation::LESS_EQUAL;

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

            case BinaryOperation::EQUAL: return "EQ";
            case BinaryOperation::NOT_EQUAL: return "NEQ";
            case BinaryOperation::LESS: return "LT";
            case BinaryOperation::LESS_EQUAL: return "LE";
            case BinaryOperation::GREATER: return "GT";
            case BinaryOperation::GREATER_EQUAL: return "GE";

            case BinaryOperation::LOGICAL_AND: return "AND";
            case BinaryOperation::LOGICAL_OR: return "OR";

            default:
                throw std::runtime_error("Missing string representation of BinaryOperation.");
        }
    }


}

#endif //IR_BINARY_OP_H
