#ifndef IR_IF_HPP
#define IR_IF_HPP
#include <memory>
#include <vector>

#include "ir_instruction.hpp"
#include "cherry/ir/values/ir_value.hpp"

namespace cherry::ir {

    struct IRIf final : IRInstruction {
        std::unique_ptr<IRValue> condition;
        std::vector<std::unique_ptr<IRInstruction>> then_body;
        std::vector<std::unique_ptr<IRInstruction>> else_body;

        IRIf(
            std::unique_ptr<IRValue> cond,
            std::vector<std::unique_ptr<IRInstruction>>&& then_b,
            std::vector<std::unique_ptr<IRInstruction>>&& else_b
        ) :
            condition(std::move(cond)),
            then_body(std::move(then_b)),
            else_body(std::move(else_b)) {}
    };

}

#endif //IR_IF_HPP
